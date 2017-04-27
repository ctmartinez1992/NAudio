#pragma once

#include "ControlGenerator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlPulse_ : public ControlGenerator_ {
		protected:
			typedef enum {
				ControlPulseStateOff = 0,
				ControlPulseStateOn
			} ControlPulseState;

			ControlPulseState state_;

			double lastOnTime_;

			ControlGenerator triggerGen_;
			ControlGenerator pulseLengthGen_;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlPulse_();

			void
			setTriggerGen(ControlGenerator gen) {
				triggerGen_ = gen;
			}

			void
			setPulseLengthGen(ControlGenerator gen) {
				pulseLengthGen_ = gen;
			}
		};

		inline void
		ControlPulse_::computeOutput(const SynthesisContext_& context) {
			ControlGeneratorOutput tickIn = triggerGen_.tick(context);
			ControlGeneratorOutput lengthIn = pulseLengthGen_.tick(context);

			output_.triggered = false;

			//Every time input changes, reset status, start new pulse.
			if(tickIn.triggered) {
				state_ = ControlPulseStateOn;
				lastOnTime_ = context.elapsedTime;
				output_.triggered = true;
				output_.value = 1.0f;
			}
			else if(state_ == ControlPulseStateOn) {
				double tDiff = context.elapsedTime - lastOnTime_;

				if(tDiff < 0 || tDiff >= Max(0.0f, lengthIn.value)) {
					state_ = ControlPulseStateOff;
					output_.value = 0.0f;
					output_.triggered = true;
				}
			}
		}
	}

	class ControlPulse : public TemplatedControlGenerator<NAudio_DSP::ControlPulse_> {
	public:
		ControlPulse(float length = 0.1f) {
			gen()->setPulseLengthGen(ControlValue(length));
		}

		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlPulse, trigger, setTriggerGen);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlPulse, length, setPulseLengthGen);
	};
}