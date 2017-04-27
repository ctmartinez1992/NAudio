#pragma once

#include "Generator.h"
#include "NRNG/NRNG.h"

namespace NAudio {
	namespace NAudio_DSP {
		class Noise_ : public Generator_ {
		protected:
			void
			computeSynthesisBlock(const SynthesisContext_& context);
		};

		inline void
		Noise_::computeSynthesisBlock(const SynthesisContext_& context) {
			float* fdata = &outputFrames_[0];

			for(unsigned int i = 0; i < outputFrames_.Size(); ++i) {
				*fdata++ = NRNG::GetInstance()->GetSample();
			}
		}

#define kNumPinkNoiseBins			16
#define kNumPinkNoiseBinsLog2		4

		//Pink noise generator. Sources:http://www.firstpr.com.au/dsp/pink-noise/ & http://vellocet.com/dsp/noise/VRand.html.
		class PinkNoise_ : public Generator_ {
		private:
			float pinkAccum_;
			float pinkBins_[kNumPinkNoiseBins];

			unsigned long pinkCount_;

			unsigned long
			countTrailingZeros(unsigned long n);

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			PinkNoise_();
		};

		inline void
		PinkNoise_::computeSynthesisBlock(const SynthesisContext_& context) {
			//TODO: stereo?
			float binval;
			float prevbinval;
			unsigned long binidx;

			float* outptr = &outputFrames_[0];

			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				binidx = countTrailingZeros(pinkCount_);
				binidx = binidx & (kNumPinkNoiseBins - 1);

				prevbinval = pinkBins_[binidx];

				while(true) {
					binval = NRNG::GetInstance()->GetSample();

					pinkBins_[binidx] = binval;

					binval -= prevbinval;
					pinkAccum_ += binval;

					if(pinkAccum_ < -kNumPinkNoiseBinsLog2 || pinkAccum_ > kNumPinkNoiseBinsLog2) {
						pinkAccum_ -= binval;
					}
					else {
						break;
					}
				}

				pinkCount_++;

				*outptr++ = (NRNG::GetInstance()->GetSample() + pinkAccum_) / (kNumPinkNoiseBinsLog2 + 1);
			}
		}

		inline unsigned long
		PinkNoise_::countTrailingZeros(unsigned long n) {
			unsigned long i = 0;

			while(((n >> i) & 1) == 0 && i < sizeof(unsigned long) * 8ul) {
				i++;
			}

			return(i);
		}
	}

	class Noise : public TemplatedGenerator<NAudio_DSP::Noise_> {
	public:
		Noise(bool stereo = false) {
			gen()->setIsStereoOutput(stereo);
		}
	};

	class PinkNoise : public TemplatedGenerator<NAudio_DSP::PinkNoise_> {
	};
}