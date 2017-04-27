#pragma once

#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlDbToLinear_ : public ControlConditioner_ {
		protected:
			void
			computeOutput(const SynthesisContext_& context);
		};

		inline void
		ControlDbToLinear_::computeOutput(const SynthesisContext_& context) {
			ControlGeneratorOutput inputOutput = input_.tick(context);

			output_.triggered = inputOutput.triggered;

			if(inputOutput.triggered) {
				output_.value = DBToLin(inputOutput.value);
			}
		}
	}

	class ControlDbToLinear : public TemplatedControlConditioner<ControlDbToLinear, NAudio_DSP::ControlDbToLinear_> {
	};
}