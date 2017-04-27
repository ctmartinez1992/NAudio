#pragma once

#include "ControlGenerator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlCounter_ : public ControlGenerator_ {
		protected:
			void
			computeOutput(const SynthesisContext_& context);

			ControlGenerator trigger;
			ControlGenerator end;

		public:
			ControlCounter_();

			void
			setTrigger(ControlGenerator gen) {
				trigger = gen;
			}
			void
			setEnd(ControlGenerator gen) {
				end = gen;
			}
		};

		inline void
		ControlCounter_::computeOutput(const SynthesisContext_& context) {
			ControlGeneratorOutput tickOut = trigger.tick(context);
			ControlGeneratorOutput endOut = end.tick(context);

			output_.triggered = tickOut.triggered;

			if(tickOut.triggered) {
				output_.value += 1.0f;

				if(output_.value > endOut.value) {
					output_.value = 0.0f;
				}
			}
		}
	}

	class ControlCounter : public TemplatedControlGenerator<NAudio_DSP::ControlCounter_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlCounter, trigger, setTrigger);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlCounter, end, setEnd);
	};
}