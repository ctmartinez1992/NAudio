#include "ControlArithmetic.h"

namespace NAudio {
	namespace NAudio_DSP {
		void
		ControlAdder_::input(ControlGenerator input) {
			inputs.push_back(input);
		}
		
		void
		ControlMultiplier_::input(ControlGenerator input) {
			inputs.push_back(input);
		}
	}
}