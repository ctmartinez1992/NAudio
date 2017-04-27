#include "SawtoothWave.h"

namespace NAudio {
	namespace NAudio_DSP {
		AngularWave_::AngularWave_() :
			phaseAccum_(0.0)
		{
			freqFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);
			slopeFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);

			slopeGen_ = FixedValue(0.0f);
			freqGen_ = FixedValue(440.0f);
		}
	}
}