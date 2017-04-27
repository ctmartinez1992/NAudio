#include "BitCrusher.h"

namespace NAudio {
	namespace NAudio_DSP {
		BitCrusher_::BitCrusher_() {
			SetBitDepth(ControlValue(16));
		}

		void
		BitCrusher_::SetInput(Generator input) {
			input_ = input;
			setIsStereoInput(input_.isStereoOutput());
		}

		void
		BitCrusher_::SetIsStereoInput(bool stereo) {
			if(stereo != isStereoInput_) {
				dryFrames_.Resize(kSynthesisBlockSize, stereo ? 2u : 1u, 0.0f);
				outputFrames_.Resize(kSynthesisBlockSize, stereo ? 2u : 1u, 0.0f);
			}

			isStereoInput_ = stereo;
			isStereoOutput_ = stereo;
		};

		void
		BitCrusher_::SetBitDepth(ControlGenerator bitDepthArg) {
			bitDepth = bitDepthArg;
		}
	}
}