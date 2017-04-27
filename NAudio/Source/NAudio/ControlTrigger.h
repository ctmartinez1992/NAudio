#pragma once

#include "ControlGenerator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlTrigger_ : public ControlGenerator_ {
		protected:
			bool doTrigger;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlTrigger_();

			void
			trigger(float value);
		};

		inline void
		ControlTrigger_::computeOutput(const SynthesisContext_& context) {
			output_.triggered = doTrigger;
			doTrigger = false;
		}
	}

	//Status changes to ControlGeneratorStatusHasChanged when trigger is called.
	class ControlTrigger : public TemplatedControlGenerator<NAudio_DSP::ControlTrigger_> {
	public:
		void
		trigger(float value = 1.0f);
	};
}