#include "MonoToStereoPanner.h"

namespace NAudio {
	namespace NAudio_DSP {
		MonoToStereoPanner_::MonoToStereoPanner_() {
			setIsStereoOutput(true);

			panFrames.Resize(kSynthesisBlockSize, 1u);

			setPan(ControlValue(0));
		}

		MonoToStereoPanner_::~MonoToStereoPanner_() {
		}

		void
		MonoToStereoPanner_::setPan(ControlGenerator panArg) {
			panControlGen = panArg;
		}
	}
}
