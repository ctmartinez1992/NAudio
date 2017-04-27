#include "ControlRecorder.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlRecorder_::ControlRecorder_() :
			playbackHead(recording.begin())
		{
		}

		void
		ControlRecorder_::setMode(ControlGenerator modeArg) {
			mode = modeArg;
		}
	}
}