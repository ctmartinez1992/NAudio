#pragma once

#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlPrinter_ : public ControlConditioner_ {
		protected:
			std::string message;
			bool hasPrinted;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlPrinter_();

			void
			setMessage(std::string message);
		};

		inline void
		ControlPrinter_::computeOutput(const SynthesisContext_& context) {
			output_ = input_.tick(context);

			if(!hasPrinted || (output_.triggered)) {
				printf(message.c_str(), output_.value);
				hasPrinted = true;
			}
		}
	}

	//A controlconditioner which can be used to print the value of its input. You can set an optional format std::string to customize the message using printf-style formatting, with exactly one %f placeholder.
	//For example: "Current bpm is %f.";.
	class ControlPrinter : public TemplatedControlConditioner<NAudio::ControlPrinter, NAudio_DSP::ControlPrinter_> {
	public:
		ControlPrinter
		message(std::string message) {
			gen()->setMessage(message);
			return(*this);
		}
	};
}