#ifndef _INT16FFT
#define _INT16FFT

#include <stdint.h>
#include "main.h"

#ifndef N_FFT
#define	N_FFT	128		/* 64, 128, 256, 512 or 1024 */
#endif

typedef struct {
	int16_t r, i;
} int16comp_t;

extern const uint16_t Tbl_brev[];

uint16_t isqrt32 (uint32_t l);
uint32_t isqrt64 (uint64_t l);
void int16fft_exec (int16comp_t* fftbuf);
void int16fft_output_scalar (int16comp_t* fftbuf, uint16_t* outbuf);

#endif
