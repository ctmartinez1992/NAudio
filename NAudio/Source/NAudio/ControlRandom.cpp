#include "ControlRandom.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlRandom_::ControlRandom_() {
			trigger = ControlValue(0);
			min = ControlValue(0);
			max = ControlValue(1);
		}
	}
}