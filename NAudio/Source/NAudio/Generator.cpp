#include "Generator.h"

namespace NAudio {
	namespace NAudio_DSP {
		Generator_::Generator_() :
			lastFrameIndex_(0), isStereoOutput_(false)
		{
			outputFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);
		}

		Generator_::~Generator_() {
		}

		void
		Generator_::setIsStereoOutput(bool stereo) {
			if(stereo != isStereoOutput_) {
				outputFrames_.Resize(kSynthesisBlockSize, stereo ? 2u : 1u, 0.0f);
			}

			isStereoOutput_ = stereo;
		}
	}
}