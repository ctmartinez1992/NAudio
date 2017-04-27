#include "Noise.h"

namespace NAudio {
	namespace NAudio_DSP {
		PinkNoise_::PinkNoise_() :
			pinkCount_(1ul), pinkAccum_(0.0f)
		{
			memset(pinkBins_, 0, kNumPinkNoiseBins * sizeof(float));
		}
	}
}