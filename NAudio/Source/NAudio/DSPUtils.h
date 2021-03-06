#pragma once

#include "NAudioCore.h"

namespace NAudio {
	//Complex Absolute Value.
	inline float
	cabs(float x, float y) {
		return(sqrtf((x * x) + (y * y)));
	}

	//Complex Exponential.
	inline void
	cexp(float x, float y, float* zx, float* zy) {
		float expx = expf(x);
		*zx = expx * cosf(y);
		*zy = expx * sinf(y);
	}

	//Hann Window generation.
	inline void
	GenerateHannWindow(int length, float* output) {
		int m = length - 1;

		for(int i = 0; i <= m; ++i) {
			output[i] = 0.5f * (1.0f - cosf((2.0f * PI * i) / m));
		}
	}

	//Hamming Window generation.
	inline void
	GenerateHammingWindow(int length, float* output) {
		int m = length - 1;

		for(int i = 0; i <= m; ++i) {
			output[i] = 0.54f - 0.46f * cosf((2.0f * PI * i) / m);
		}
	}

	//Blackman Window generation.
	inline void
	GenerateBlackmanWindow(int length, float* output) {
		int m = length - 1;
		float f1;
		float f2;
		float fm = (float)m;

		for(int i = 0; i <= m; ++i) {
			f1 = (2.0f * PI * (float)i) / fm;
			f2 = 2.0f * f1;

			output[i] = 0.42f - (0.5f * cosf(f1)) + (0.08f * cosf(f2));
		}
	}

	//Discrete Fourier Transform. Non-FFT, brute force approach intended for wavetable generation.
	//NOTE: Do not use for real-time processing.
	void
	DFT(int length, float* realTimeIn, float* imagTimeIn, float* realFreqOut, float* imagFreqOut);

	//Inverse Discrete Fourier Transform. Non-FFT, brute force approach intended for wavetable generation.
	//NOTE: Do not use for real-time processing.
	void
	InverseDFT(int length, float* realFreqIn, float* imagFreqIn, float* realTimeOut, float* imagTimeOut);

	//Real Cepstrum.
	void
	RealCepstrum(int length, float* signalIn, float* realCepstrumOut);

	//Compute Minimum Phase Reconstruction of signal from its real cepstrum.
	void
	MinimumPhase(int n, float* realCepstrum, float* minimumPhase);

	//Generate minBlep.
	//Returns pointer to buffer of length * lengthOut. Caller is responsible for freeing heap-allocated buffer.
	//This can be used to create a new minimum phase bandlimited step function to store for later use as a lookup table.
	//Note that BLEPOscillator.cpp already has a runtime constant version of a BLEP for use with the BLEP - based oscillators.
	float*
	GenerateMinBLEP(int zeroCrossings, int overSampling);
}