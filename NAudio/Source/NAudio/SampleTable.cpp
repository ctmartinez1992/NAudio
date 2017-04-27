#include "SampleTable.h"

namespace NAudio {
	namespace NAudio_DSP {
		SampleTable_::SampleTable_(unsigned int frames, unsigned int channels) {
			//Limited to 2 channels.
			frames_.Resize(frames, Min(channels, 2));
		}
	}
}