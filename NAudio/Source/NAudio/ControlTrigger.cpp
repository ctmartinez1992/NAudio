#include "ControlTrigger.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlTrigger_::ControlTrigger_() :
			doTrigger(false)
		{
		}

		void
		ControlTrigger_::trigger(float value) {
			doTrigger = true;
			output_.value = value;
		}
	} 

	void
	ControlTrigger::trigger(float value) {
		gen()->trigger(value);
	}
}