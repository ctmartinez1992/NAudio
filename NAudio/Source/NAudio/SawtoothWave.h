#pragma once

#include "Generator.h"
#include "BLEPOscillator.h"

#define NAUDIO_SAW_RES 4096

namespace NAudio {
	namespace NAudio_DSP {
		//Generator_ class to generate hard-edge rising saw, falling saw, triangle, or any angle in-between.
		class AngularWave_ : public Generator_ {
		protected:
			Generator freqGen_;
			Generator slopeGen_;

			NAudioFrames freqFrames_;
			NAudioFrames slopeFrames_;

			double phaseAccum_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			AngularWave_();

			void
			setFrequencyGenerator(Generator gen) {
				freqGen_ = gen;
			}

			void
			setSlopeGenerator(Generator gen) {
				slopeGen_ = gen;
			}
		};

		inline void
		AngularWave_::computeSynthesisBlock(const SynthesisContext_& context) {
			//Tick freq and slope inputs.
			freqGen_.tick(freqFrames_, context);
			slopeGen_.tick(slopeFrames_, context);

			//Calculate the output wave.
			float const rateConstant = NAUDIO_SAW_RES / NAudio::SampleRate();

			float slope;
			float frac;
			float phase;

			float* outptr = &outputFrames_[0];
			float* freqptr = &freqFrames_[0];
			float* slopeptr = &slopeFrames_[0];

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
				//Update the slope.
				slope = Clamp(*slopeptr++, 0.0f, 1.0f) * NAUDIO_SAW_RES;

				sd.d = ps;
				ps += *freqptr++;
				offs = sd.i[1] & (NAUDIO_SAW_RES - 1);
				sd.i[1] = msbi;
				frac = (float)(sd.d - BIT32DECPT);

				phase = offs + frac;

				//TODO: This is still a bit slow. Maybe find a way to do it with a real table lookup.
				//On the rising edge.
				if(phase < slope) {
					*outptr++ = (phase / slope) * 2.0f - 1.0f;
				}
				//On the falling edge.
				else {
					*outptr++ = (1.0f - ((phase - slope) / (NAUDIO_SAW_RES - slope))) * 2.0f - 1.0f;
				}
			}

			sd.d = BIT32DECPT * NAUDIO_SAW_RES;
			msbi = sd.i[1];
			sd.d = ps + (BIT32DECPT * NAUDIO_SAW_RES - BIT32DECPT);
			sd.i[1] = msbi;
			phaseAccum_ = sd.d - BIT32DECPT * NAUDIO_SAW_RES;
		}

		//Anti-aliased BLEP sawtooth.
		class SawtoothWaveBL_ : public BLEPOscillator_ {
		protected:
			void
			computeSynthesisBlock(const SynthesisContext_& context);
		};

		inline void
		SawtoothWaveBL_::computeSynthesisBlock(const NAudio_DSP::SynthesisContext_& context) {
			static const float rateConstant = 1.0f / NAudio::SampleRate();

			//Tick freq and pwm.
			freqGen_.tick(freqFrames_, context);

			float* outptr = &outputFrames_[0];
			float* freqptr = &freqFrames_[0];

			//Pre-multiply rate constant for speed.
			for(unsigned int i = 0; i < kSynthesisBlockSize; i++) {
				*freqptr++ *= rateConstant;
			}

			freqptr = &freqFrames_[0];

			//TODO: Maybe do this using a fast phasor for wraparound speed.
			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i, ++freqptr, ++outptr) {
				phase_ += *freqptr;

				//Add BLEP at end.
				if(phase_ >= 1.0f) {
					phase_ -= 1.0f;
					accum_ = 0.0f;

					addBLEP(phase_ / (*freqptr), 1.0f);
				}

				*outptr = (float)phase_;

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

	//Quick-and-dirty sawtooth oscillator. Not anti-aliased, so really best as an LFO. Can be used as an audio sawtooth oscillator in a pinch or if you don't mind some aliasing distortion.
	class SawtoothWave : public TemplatedGenerator<NAudio_DSP::AngularWave_> {
	public:
		NAUDIO_MAKE_GEN_SETTERS(SawtoothWave, freq, setFrequencyGenerator);

		//Set whether it's a descending sawtooth (default) or ascending.
		SawtoothWave&
		isAscending(bool ascending) {
			gen()->setSlopeGenerator(FixedValue(ascending ? 1.0f : 0.0f));
			return(*this);
		}
	};

	//Bandlimited sawtooth generator.
	class SawtoothWaveBL : public TemplatedGenerator<NAudio_DSP::SawtoothWaveBL_> {
	public:
		NAUDIO_MAKE_GEN_SETTERS(SawtoothWaveBL, freq, setFreqGen);
	};
}