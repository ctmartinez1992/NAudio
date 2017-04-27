#include "ControlStepper.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlStepper_::ControlStepper_() :
			direction(1),
			hasBeenTriggered(false)
		{
			start = ControlValue(0.0f);
			end = ControlValue(1.0f);
			step = ControlValue(1.0f);
			bidirectional = ControlValue(0.0f);
		}
	}
}