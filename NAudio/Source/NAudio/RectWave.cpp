#include "RectWave.h"

namespace NAudio {
	namespace NAudio_DSP {

		RectWave_::RectWave_() :
			phaseAccum_(0.0)
		{
			pwmGen_ = FixedValue(0.5f);
			freqFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);
			pwmFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);
		}

		RectWaveBL_::RectWaveBL_() {
			pwmGen_ = FixedValue(0.5f);
			pwmFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);
		}
	}
}