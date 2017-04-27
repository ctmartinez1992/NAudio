#pragma once

#include "Generator.h"
#include "NRNG/NRNG.h"

namespace NAudio {
	namespace NAudio_DSP {
		class LFNoise_ : public Generator_ {
		protected:
			NAudioFrames mFreqFrames;

			float mSlope;
			float mLevel;
			signed long mCounter;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			ControlGenerator mFreq;

			LFNoise_();

			void
			setFreq(ControlGenerator freq);
		};

		inline void
		LFNoise_::computeSynthesisBlock(const SynthesisContext_& context) {
			unsigned long remain = (unsigned int)outputFrames_.Frames();

			float* out = &outputFrames_[0];

			do {
				if(mCounter <= 0) {
					mCounter = (long)(SampleRate() / std::max<float>(mFreq.tick(context).value, .001f));
					mCounter = (long)(std::max<float>(1.0f, (float)mCounter));

					float nextlevel = NRNG::GetInstance()->GetFloat(-1.0f, 1.0f);

					mSlope = (nextlevel - mLevel) / mCounter;
				}

				unsigned long nsmps = std::min(remain, (unsigned long)mCounter);

				remain -= nsmps;
				mCounter -= nsmps;

				for(unsigned long i = 0; i < nsmps; ++i) {
					*(out++) = mLevel;
					mLevel += mSlope;
				}

			} while(remain);
		}
	}

	class LFNoise : public TemplatedGenerator<NAudio_DSP::LFNoise_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(LFNoise, setFreq, setFreq);
	};
}