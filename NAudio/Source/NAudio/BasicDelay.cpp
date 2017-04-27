#include "BasicDelay.h"

namespace NAudio {
	namespace NAudio_DSP {

		BasicDelay_::BasicDelay_() {
			delayTimeFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);
			fbkFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);

			delayTimeGen_ = FixedValue(0.0f);
			fbkGen_ = FixedValue(0.0f);
			setDryLevelGen(FixedValue(0.5f));
			setWetLevelGen(FixedValue(0.5f));
		}

		BasicDelay_::~BasicDelay_() {
		}

		void
		BasicDelay_::setInput(Generator input) {
			Effect_::setInput(input);
			setIsStereoInput(input.isStereoOutput());
			setIsStereoOutput(input.isStereoOutput());

			//Can safely resize as NAudioFrames subclass - calling functions account for channel offset.
			delayLine_.Resize(delayLine_.Frames(), input.isStereoOutput() ? 2u : 1u, 0.0f);
		}

		void
		BasicDelay_::initialize(float delayTime, float maxDelayTime) {
			if(maxDelayTime <= 0.0f) {
				maxDelayTime = delayTime * 1.5f;
			}

			delayLine_.initialize(maxDelayTime, 1u);
			delayTimeGen_ = FixedValue(delayTime);
		}
	}

	BasicDelay::BasicDelay(float initialDelayTime, float maxDelayTime) {
		gen()->initialize(initialDelayTime, maxDelayTime);
		delayTime(initialDelayTime);
	}
}