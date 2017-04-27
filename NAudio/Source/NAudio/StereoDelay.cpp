#include "StereoDelay.h"

namespace NAudio {
	namespace NAudio_DSP {
		StereoDelay_::StereoDelay_() {
			setIsStereoOutput(true);
			setIsStereoInput(true);
			
			delayTimeFrames_[NAUDIO_LEFT].Resize(kSynthesisBlockSize, 1, 0);
			delayTimeFrames_[NAUDIO_RIGHT].Resize(kSynthesisBlockSize, 1, 0);
			
			fbkFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);

			setFeedback(FixedValue(0.0f));
			setDryLevelGen(FixedValue(0.5f));
			setWetLevelGen(FixedValue(0.5f));
		}

		void
		StereoDelay_::initialize(float leftDelayArg, float rightDelayArg, float maxDelayLeft, float maxDelayRight) {
			if(maxDelayLeft <= 0.0f) {
				maxDelayLeft = leftDelayArg * 1.5f;
			}
			if(maxDelayRight <= 0.0f) {
				maxDelayRight = rightDelayArg * 1.5f;
			}

			delayLine_[NAUDIO_LEFT].initialize(maxDelayLeft, 1u);
			delayLine_[NAUDIO_RIGHT].initialize(maxDelayRight, 1u);
		}
	}

	StereoDelay::StereoDelay(float leftDelay, float rightDelay, float maxDelayLeft, float maxDelayRight) {
		gen()->initialize(leftDelay, rightDelay, maxDelayLeft, maxDelayRight);
		delayTimeLeft(leftDelay);
		delayTimeRight(rightDelay);
	}
}