#include "ControlDelay.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlDelay_::ControlDelay_() :
			readHead_(0), writeHead_(0), maxDelay_(0)
		{
		}

		void
		ControlDelay_::initialize(float maxDelayTime) {
			maxDelay_ = Max(maxDelayTime * SampleRate() / kSynthesisBlockSize, 1.0f);
			delayLine_.resize(maxDelay_);
			readHead_ = maxDelay_ - 1;
		}
	}

	ControlDelay::ControlDelay(float maxDelayTime) {
		this->gen()->initialize(maxDelayTime);
	}
}