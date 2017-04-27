#include "Mixer.h"

namespace NAudio {
	namespace NAudio_DSP {
		Mixer_::Mixer_() {
			workSpace_.Resize(kSynthesisBlockSize, 2u, 0.0f);
		}

		void
		Mixer_::addInput(BufferFiller input) {
			//No checking for duplicates, maybe we should.
			inputs_.push_back(input);
		}

		void
		Mixer_::removeInput(BufferFiller input) {
			vector<BufferFiller>::iterator it = std::find(inputs_.begin(), inputs_.end(), input);

			if(it != inputs_.end()) {
				inputs_.erase(it);
			}
		}
	}
}