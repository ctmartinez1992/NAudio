#include "FixedValue.h"

namespace NAudio {
	namespace NAudio_DSP {
		FixedValue_::FixedValue_(float f) {
			valueGen = ControlValue(f);
		}
	}
}