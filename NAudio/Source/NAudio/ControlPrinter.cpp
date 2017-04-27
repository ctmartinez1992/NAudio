#include "ControlPrinter.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlPrinter_::ControlPrinter_() :
			message("%f\n"),
			hasPrinted(false)
		{
		}

		void
		ControlPrinter_::setMessage(std::string messageArg) {
			message = "NAudio::ControlPrinter message:" + messageArg + "\n";
		}
	}
}