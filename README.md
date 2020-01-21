#ATMega8 AVR Spectrum Analyzer

Simple spectrum analyzer based on ATMega8 microcontroller. Rather a proof-of-concept (or cool gadget) than a really useful device. Written in C.

![](analyzer.jpg)

### Features

- 128-point Fast Fourier Transform based on Cooley-Tukey radix-2 DIT algorithm
- Computation done in Q6.9 fixed point for performance
- ADC sampling rate: ~16kHz
- ADC resolution: 8-bit
- Bandwidth: ~50-8000Hz
- Spectrum presented on HD44780 16x2 LCD