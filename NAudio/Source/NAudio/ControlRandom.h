#pragma once

#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlRandom_ : public ControlGenerator_ {
		private:
			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlGenerator max;
			ControlGenerator min;
			ControlGenerator trigger;

			ControlRandom_();

			void
			setMax(ControlGenerator maxArg) {
				max = maxArg;
			}

			void
			setMin(ControlGenerator minArg) {
				min = minArg;
			}

			void
			setTrigger(ControlGenerator arg) {
				trigger = arg;
			}
		};
		
		inline void
		ControlRandom_::computeOutput(const SynthesisContext_& context) {
			ControlGeneratorOutput minOut = min.tick(context);
			ControlGeneratorOutput maxOut = max.tick(context);

			bool outInRange = (output_.value >= minOut.value) && (output_.value <= maxOut.value);

			if(!outInRange || trigger.tick(context).triggered) {
				output_.triggered = true;
				output_.value = NRNG::GetInstance()->GetFloat(minOut.value, maxOut.value);
			}
			else {
				output_.triggered = false;
			}
		}
	}

	class ControlRandom : public TemplatedControlGenerator<NAudio_DSP::ControlRandom_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlRandom, max, setMax)
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlRandom, min, setMin)
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlRandom, trigger, setTrigger)
	};
}