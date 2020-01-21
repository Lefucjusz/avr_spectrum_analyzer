#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "lcd.h"
#include "fixed_point.h"

#define AUDIO_PIN (1<<PC0)
#define AUDIO_DDR DDRC

#define ADC_OFFSET 128

#define LED_PIN (1<<PD0)

#define FFT_LENGTH 128
#define NEG_SIN_OFFSET (FFT_LENGTH/4)

#define BINS_NUMBER 16

#define INPUT_GAIN 32
#define BIN_GAIN 64

#define LPF_ALPHA 0.5 //LPF coefficient 0-1, lower values result in faster display response

typedef struct
{
	fp_t re;
	fp_t im;
} complex_t;

complex_t buffer[FFT_LENGTH];

static const uint8_t bins_numbers[BINS_NUMBER] =
{1, 2, 4, 6, 8, 12, 15, 19, 23, 27, 31, 35, 38, 46, 54, 62}; // Numbers of bins to be displayed

const fp_t cos_lut[FFT_LENGTH] = {
256,255,254,253,251,248,244,241,
236,231,225,219,212,205,197,189,
181,171,162,152,142,131,120,109,
97,86,74,62,49,37,25,12,
0,-12,-25,-37,-49,-62,-74,-86,
-97,-109,-120,-131,-142,-152,-162,-171,
-181,-189,-197,-205,-212,-219,-225,-231,
-236,-241,-244,-248,-251,-253,-254,-255,
-256,-255,-254,-253,-251,-248,-244,-241,
-236,-231,-225,-219,-212,-205,-197,-189,
-181,-171,-162,-152,-142,-131,-120,-109,
-97,-86,-74,-62,-49,-37,-25,-12,
0,12,25,37,49,62,74,86,
97,109,120,131,142,152,162,171,
181,189,197,205,212,219,225,231,
236,241,244,248,251,253,254,255};

void adc_init(void)
{
	AUDIO_DDR &= ~AUDIO_PIN; // Audio pin as input
	ADMUX = (1<<REFS1) | (1<<REFS0) | (1<<ADLAR); // Vref as reference, left-shift result, input 0
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1); //Enable ADC, single mode, prescaler 64 (~217kHz@13.875MHz XTAL)
}

void fft_acquire_samples(void)
{
	uint8_t i;
	int32_t temp;
	for(i = 0; i < FFT_LENGTH; i++)
	{
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC))
			continue;

		temp = ADCH - ADC_OFFSET; // Store sample
		temp *= INPUT_GAIN; // Apply gain
		temp *= FP_ONE; // Convert to fixed-point
		temp >>= 7; // Normalize from (-128;128) to (-1;1)
		buffer[i].re = temp; // Copy sample to buffer
		buffer[i].im = 0;
	}
}

void fft_window_samples(void) // Hann window
{
	for(uint8_t i = 0; i < FFT_LENGTH; i++)
	{
		fp_t w_n;
		w_n = fp_sub(FP_ONE, cos_lut[i]);
		w_n = fp_mul(FP_ONE_HALF, w_n);
		buffer[i].re = fp_mul(buffer[i].re, w_n);
	}
}

void swap(fp_t* x, fp_t* y) // XOR swap
{
	*x ^= *y;
	*y ^= *x;
	*x ^= *y;
}

void fft_reverse_bits(void)
{
	uint8_t j = 0, k;
	for (uint8_t i = 0; i < FFT_LENGTH - 1; i++)
	{
		if (i < j) //swap values
	    {
			swap(&buffer[i].re, &buffer[j].re); // Only real parts for performance
	    }
	    k = FFT_LENGTH >> 1;
	    while (j >= k)
	    {
	      j -= k;
	      k >>= 1;
	    }
	    j += k;
	 }
}

void fft_cooley_tukey(void) // Cooley-Tukey decimation-in-time radix-2 in-place FFT
{
	uint8_t half_size, lut_step, l;
	complex_t tp;
	for(uint16_t size = 2; size <= FFT_LENGTH; size <<= 1)
	{
		half_size = size >> 1;
		lut_step = FFT_LENGTH / size;
		for (uint8_t i = 0; i < FFT_LENGTH; i += size)
		{
			for (uint8_t j = i, k = 0; j < i + half_size; j++, k += lut_step)
			{
				l = j + half_size;
				tp.re = fp_sub(fp_mul(buffer[l].re, cos_lut[k]), fp_mul(buffer[l].im, cos_lut[k+NEG_SIN_OFFSET]));
				tp.im = fp_add(fp_mul(buffer[l].im, cos_lut[k]), fp_mul(buffer[l].re, cos_lut[k+NEG_SIN_OFFSET]));
				buffer[l].re = fp_sub(buffer[j].re, tp.re);
				buffer[l].im = fp_sub(buffer[j].im, tp.im);
				buffer[j].re = fp_add(buffer[j].re, tp.re);
				buffer[j].im = fp_add(buffer[j].im, tp.im);
			}
		}
	}
	for(uint8_t i = 0; i < FFT_LENGTH/2; i++) //normalization (half for performance)
	{
		buffer[i].re /= FFT_LENGTH;
		buffer[i].im /= FFT_LENGTH;
	}
}

void fft_compute(void)
{
	fft_reverse_bits(); // Bit-reverse input samples
	fft_cooley_tukey(); // Perform FFT using Cooley-Tukey radix-2 algorithm
}

void fft_single_sided_magnitude(void) // Super evil math hacking to avoid sqrt
{// Based on approximation sqrt(x^2+y^2) ~= max(abs(x),abs(y)) + min(abs(x),abs(y))/2
	fp_t re_abs, im_abs;
	for(uint8_t i = 0; i < FFT_LENGTH/2; i++)
	{
		re_abs = fp_abs(buffer[i].re);
		im_abs = fp_abs(buffer[i].im);
		if(re_abs > im_abs)
		{
			buffer[i].re = re_abs + (im_abs >> 1);
		}
		else
		{
			buffer[i].re = im_abs + (re_abs >> 1);
		}
	}
}

fp_t fft_lpf(fp_t x, uint8_t i) // Moving average low pass filter
{
	static fp_t y[BINS_NUMBER]; // Static array to hold previous bin value
	y[i] = fp_mul(LPF_ALPHA*FP_ONE, y[i]) + fp_mul((1 - LPF_ALPHA)*FP_ONE, x); //y = alpha * y + (1-alpha)*x
	return y[i];
}

void fft_display(void)
{
	fp_long_t bin_val;
	uint8_t bin_idx;
	for(uint8_t i = 0; i < FFT_LENGTH/8; i++)
	{
		bin_idx = bins_numbers[i]; // Get index of next FFT bin to be displayed
		if(i < 5) // If below fifth bin, span between bins is too small to compute average
		{
			bin_val = BIN_GAIN * buffer[bin_idx].re / FP_ONE; // Just take value as it is
		}
		else // Otherwise compute mean from bin itself and its neighbors
		{
			bin_val = buffer[bin_idx-1].re + buffer[bin_idx].re + buffer[bin_idx+1].re;
			bin_val = fp_div(bin_val, 3*FP_ONE);
			bin_val *= BIN_GAIN;
			bin_val /= FP_ONE;
		}
		bin_val = fft_lpf(bin_val, i); // Low pass filter to control display dynamics
		lcd_print_bin(i+1, (uint8_t)bin_val);
	}
}

void main(void)
{
	adc_init();
	lcd_init();
	lcd_load_chars();
	lcd_cls();
	lcd_string("ATMega8 FFT");
	lcd_gotoxy(2,1);
	lcd_string("BW: 50-8000Hz");
	_delay_ms(2000);
	while(1)
	{
		fft_acquire_samples();
		fft_window_samples();
		fft_compute();
		fft_single_sided_magnitude();
		fft_display();
		PORTD ^= LED_PIN; // Main loop speed test
	}
}
