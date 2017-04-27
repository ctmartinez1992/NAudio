#include "LFNoise.h"

namespace NAudio {
	namespace NAudio_DSP {
		LFNoise_::LFNoise_() :
			mCounter(0)
		{
			mFreqFrames.Resize(kSynthesisBlockSize);
		}

		void
		LFNoise_::setFreq(ControlGenerator freq) {
			mFreq = freq;
		}
	}
}
