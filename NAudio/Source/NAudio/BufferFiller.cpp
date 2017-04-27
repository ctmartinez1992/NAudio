
#include "BufferFiller.h"

namespace NAudio {
	namespace NAudio_DSP {
		BufferFiller_::BufferFiller_() :
			bufferReadPosition_(0)
		{
			NAUDIO_MUTEX_INIT(mutex_);
			setIsStereoOutput(true);
		}

		BufferFiller_::~BufferFiller_() {
			NAUDIO_MUTEX_DESTROY(mutex_);
		}
	}
}