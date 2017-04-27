#pragma once

#include "NAudioFrames.h"

namespace NAudio {

	//Calculate coefficient for a pole with given time constant to reach -60dB delta in t60s seconds.
	inline static float
	t60ToOnePoleCoef(float t60s) {
		float coef = expf(-1.0f / ((t60s / 6.91f) * SampleRate()));
		return((coef == coef) ? coef : 0.0f);		//NaN checking.
	}

	//Calculate coefficient for a pole with a given desired cutoff in hz.
	inline static float
	cutoffToOnePoleCoef(float cutoffHz) {
		return(Clamp(expf(-TWO_PI*cutoffHz / SampleRate()), 0.0f, 1.0f));
	}

	//Tick one sample through one-pole lowpass filter.
	inline void
	onePoleLPFTick(float input, float& output, float coef) {
		output = ((1.0f - coef) * input) + (coef * output);
	}

	//Tick one sample through one-pole highpass filter.
	inline void
	onePoleHPFTick(float input, float& output, float coef) {
		output = ((1.0f - coef) * input) - (coef * output);
	}

	//Compute coefficients from analog prototype using bilinear transform.
	//Analog transfer function (laplace domain) should look like:
	//		  b2 s^2 + b1 s + b0
	//H(s) = --------------------
	//		   s^2 + a1 s + a0
	//And be normalized for a cutoff of 1 rad/s. fc is the desired frequency cutoff in Hz. coef_out is a pointer to a float array of length 5. No bounds checking is performed.
	inline static void
	bltCoef(float b2, float b1, float b0, float a1, float a0, float fc, float* coef_out) {
		float sf = 1.0f / tanf(PI * fc / NAudio::SampleRate());
		float sfsq = sf * sf;
		float norm = a0 + a1 * sf + sfsq;

		coef_out[0] = (b0 + b1 * sf + b2 * sfsq) / norm;
		coef_out[1] = 2.0f * (b0 - b2 * sfsq) / norm;
		coef_out[2] = (b0 - b1 * sf + b2 * sfsq) / norm;
		coef_out[3] = 2.0f * (a0 - sfsq) / norm;
		coef_out[4] = (a0 - a1 * sf + sfsq) / norm;
	}

	//Biquad Class. Biquad_ is an IIR biquad filter which provides a base object on which to build more advanced filters.
	class Biquad {
	protected:
		float coef_[5];

		NAudioFrames inputVec_;
		NAudioFrames outputVec_;

	public:
		Biquad();

		void
		setIsStereo(bool stereo) {
			//Resize vectors to match number of channels.
			inputVec_.Resize(kSynthesisBlockSize + 4, stereo ? 2u : 1u, 0.0f);
			outputVec_.Resize(kSynthesisBlockSize + 4, stereo ? 2u : 1u, 0.0f);
		}

		//Set the coefficients for the filtering operation.
		//		  b0 + b1*z^-1 + b2*z^-2
		//H(z) = ------------------------
		//		  1 + a1*z^-1 + a2*z^-2
		void
		setCoefficients(float b0, float b1, float b2, float a1, float a2);
		void
		setCoefficients(float* newCoef);

		void
		filter(NAudioFrames& inFrames, NAudioFrames& outFrames);
	};

	inline void
	Biquad::setCoefficients(float b0, float b1, float b2, float a1, float a2) {
		coef_[0] = b0;
		coef_[1] = b1;
		coef_[2] = b2;
		coef_[3] = a1;
		coef_[4] = a2;
	}

	inline void
	Biquad::setCoefficients(float* newCoef) {
		memcpy(coef_, newCoef, 5 * sizeof(float));
	}

	inline void
	Biquad::filter(NAudioFrames& inFrames, NAudioFrames& outFrames) {
		//Initialize vectors.
		memcpy(&inputVec_[0], &inputVec_(kSynthesisBlockSize, 0u), 2 * inputVec_.Channels() * sizeof(float));
		memcpy(&inputVec_(2, 0u), &inFrames[0], inFrames.Size() * sizeof(float));
		memcpy(&outputVec_[0], &outputVec_(kSynthesisBlockSize, 0u), 2 * outputVec_.Channels() * sizeof(float));

		//Perform IIR filter.
		unsigned int stride = inFrames.Channels();

		for(unsigned int c = 0; c < stride; ++c) {
			float* in = &inputVec_(2, c);
			float* out = &outputVec_(2, c);

			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				*out = *(in)*coef_[0] + *(in - stride)*coef_[1] + *(in - 2 * stride)*coef_[2] - *(out - stride)*coef_[3] - *(out - 2 * stride)*coef_[4];

				in += stride;
				out += stride;
			}
		}

		if(outFrames(0, 0u) != outFrames(0, 0u)) {
			LOG(NLOG_ERROR, "NaN detected.", false);
		}

		//Copy to synthesis block.
		memcpy(&outFrames[0], &outputVec_(2, 0u), kSynthesisBlockSize * stride * sizeof(float));
	}
};