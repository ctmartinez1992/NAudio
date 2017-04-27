#pragma once

#include "ControlGenerator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlTriggerFilter_ : public ControlGenerator_ {
		protected:
			std::vector<bool> sequence_;

			ControlGenerator trigger_;
			ControlGenerator loop_;

			int step;

			bool finished;		//For cases when loop is false.

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlTriggerFilter_();
			~ControlTriggerFilter_();

			void
			sequence(std::string);

			void
			trigger(ControlGenerator);

			void
			setLoop(ControlGenerator);
		};
	}

	//Passes through, or blocks trigger messages based on a std::string-encoded sequence of "0" and "1" passed to the "sequence" method.
	//"1110" will result in four passes and one block. "111000" will result in three passes and three blocks.
	//Any character other than "1" and "0" will be ignored. So you can add delimiters for readability, like this: "1110|1000|1010|1111". "1" means pass, "0" means block.
	class ControlTriggerFilter : public TemplatedControlGenerator<NAudio_DSP::ControlTriggerFilter_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlTriggerFilter, trigger, trigger);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlTriggerFilter, loop, setLoop);

		ControlTriggerFilter
		sequence(std::string seqArg) {
			gen()->sequence(seqArg);
			return(*this);
		}
	};
}