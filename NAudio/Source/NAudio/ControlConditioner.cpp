#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		void
		ControlConditioner_::input(ControlGenerator input) {
			input_ = input;
		}
	}
}