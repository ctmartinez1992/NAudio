#include "ControlCounter.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlCounter_::ControlCounter_() {
			end = ControlValue(std::numeric_limits<float>::infinity());
		}
	}
}