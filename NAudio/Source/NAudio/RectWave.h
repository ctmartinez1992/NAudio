#pragma once

#include "Generator.h"
#include "BLEPOscillator.h"

#define NAUDIO_RECT_RES 4096

namespace NAudio {
	namespace NAudio_DSP {
		//Quick-and-dirty non-bandlimited rect wave.
		class RectWave_ : public Generator_ {
		protected:
			Generator freqGen_;
			Generator pwmGen_;

			NAudioFrames freqFrames_;
			NAudioFrames pwmFrames_;

			double phaseAccum_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			RectWave_();

			void
			setFrequencyGenerator(Generator gen) {
				freqGen_ = gen;
			}

			void
			setPwmGenerator(Generator gen) {
				pwmGen_ = gen;
			}
		};

		inline void
		RectWave_::computeSynthesisBlock(const SynthesisContext_& context) {
			//Tick freq and pwm.
			freqGen_.tick(freqFrames_, context);
			pwmGen_.tick(pwmFrames_, context);

			const float rateConstant = NAUDIO_RECT_RES / NAudio::SampleRate();

			float* outptr = &outputFrames_[0];
			float* freqptr = &freqFrames_[0];
			float* pwmptr = &pwmFrames_[0];

			FastPhasor sd;

			//Pre-multiply rate constant for speed.
			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				*freqptr++ *= rateConstant;
			}

			freqptr = &freqFrames_[0];

			sd.d = BIT32DECPT;

			int offs;
			int msbi = sd.i[1];
			double ps = phaseAccum_ + BIT32DECPT;

			for(unsigned int i = 0; i < outputFrames_.Frames(); ++i) {
				sd.d = ps;
				ps += *freqptr++;
				offs = sd.i[1] & (NAUDIO_RECT_RES - 1);
				sd.i[1] = msbi;

				*outptr++ = offs > (NAUDIO_RECT_RES * *pwmptr++) ? -1.0f : 1.0f;
			}

			sd.d = BIT32DECPT * NAUDIO_RECT_RES;
			msbi = sd.i[1];
			sd.d = ps + (BIT32DECPT * NAUDIO_RECT_RES - BIT32DECPT);
			sd.i[1] = msbi;

			phaseAccum_ = sd.d - BIT32DECPT * NAUDIO_RECT_RES;
		}

		//Bandlimited Rect Wave.
		class RectWaveBL_ : public BLEPOscillator_ {
		protected:
			//Input Generators.
			Generator pwmGen_;
			NAudioFrames pwmFrames_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			RectWaveBL_();

			void
			setPWMGen(Generator gen) {
				pwmGen_ = gen;
			}
		};

		inline void
		RectWaveBL_::computeSynthesisBlock(const NAudio_DSP::SynthesisContext_& context) {
			static const float rateConstant = 1.0f / NAudio::SampleRate();

			//Tick freq and pwm.
			freqGen_.tick(freqFrames_, context);
			pwmGen_.tick(pwmFrames_, context);

			float* outptr = &outputFrames_[0];
			float* freqptr = &freqFrames_[0];
			float* pwmptr = &pwmFrames_[0];

			//Pre-multiply rate constant for speed.
			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				*freqptr++ *= rateConstant;
			}

			freqptr = &freqFrames_[0];

			//TODO: Maybe do this using a fast phasor for wraparound speed.
			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i, ++pwmptr, ++freqptr, ++outptr) {
				phase_ += *freqptr;

				//Add BLEP at end.
				if(phase_ >= 1.0f) {
					phase_ -= 1.0f;
					accum_ = 0.0f;

					addBLEP(phase_ / (*freqptr), 1.0f);
				}

				//Add BLEP when we exceed pwm.
				if(accum_ == 0.0f && phase_ > *pwmptr) {
					accum_ = 1.0f;

					addBLEP((phase_ - (*pwmptr)) / (*freqptr), -1.0f);
				}

				*outptr = accum_;

				//Add BLEP buffer contents.
				if(nInit_ > 0) {
					*outptr += ringBuf_[iBuffer_];
					nInit_--;

					if(++iBuffer_ >= lBuffer_) {
						iBuffer_ = 0;
					}
				}

				//Remove DC offset.
				*outptr = (*outptr * 2.0f) - 1.0f;
			}
		}
	}

	//Quick-and-dirty rectangular wave.
	//NOTE: Is NOT anti-aliased! Best for use as LFO, or if you just don't give a shit...
	class RectWave : public TemplatedGenerator<NAudio_DSP::RectWave_> {
	public:
		//Set the frequency of the waveform.
		NAUDIO_MAKE_GEN_SETTERS(RectWave, freq, setFrequencyGenerator);

		//Set the pulse width of the rectangle. Input should be clipped between 0-1.
		NAUDIO_MAKE_GEN_SETTERS(RectWave, pwm, setPwmGenerator);
	};

	//Bandlimited rectangular wave.
	class RectWaveBL : public TemplatedGenerator<NAudio_DSP::RectWaveBL_> {
	public:
		//Set the frequency of the waveform.
		NAUDIO_MAKE_GEN_SETTERS(RectWaveBL, freq, setFreqGen);

		//Set the pulse width of the rectangle. Input should be clipped between 0-1.
		NAUDIO_MAKE_GEN_SETTERS(RectWaveBL, pwm, setPWMGen);
	};
}