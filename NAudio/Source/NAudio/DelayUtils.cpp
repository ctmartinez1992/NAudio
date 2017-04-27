#include "DelayUtils.h"

namespace NAudio {
	DelayLine::DelayLine() :
		lastDelayTime_(0.0f), readHead_(0.0f), writeHead_(0l),
		isInitialized_(false), interpolates_(true)
	{
		Resize(kSynthesisBlockSize, 1u, 0.0f);
	}

	void
	DelayLine::initialize(float maxDelay, unsigned int channels) {
		unsigned int nFrames = (unsigned int)Max(2.0f, maxDelay * NAudio::SampleRate());

		Resize(nFrames, channels, 0.0f);

		isInitialized_ = true;
	}

	void
	DelayLine::clear() {
		if(isInitialized_) {
			memset(data, 0, size * sizeof(float));
		}
	}
}