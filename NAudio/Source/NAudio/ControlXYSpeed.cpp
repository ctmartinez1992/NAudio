#include "ControlXYSpeed.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlXYSpeed_::ControlXYSpeed_() :
			lastXVal(0.0f), lastYVal(0.0f)
		{
		}

		void
		ControlXYSpeed_::setX(ControlGenerator xArg) {
			x = xArg;
		}

		void
		ControlXYSpeed_::setY(ControlGenerator yArg) {
			y = yArg;
		}
	}
}