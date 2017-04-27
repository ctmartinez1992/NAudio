#pragma once

#include <iostream>
#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlMidiToFreq_ : public ControlConditioner_ {
			void
			computeOutput(const SynthesisContext_& context) {
				ControlGeneratorOutput inputOut = input_.tick(context);

				output_.triggered = inputOut.triggered;
				output_.value = MtoF(inputOut.value);
			}
		};
	}

	class ControlMidiToFreq : public TemplatedControlConditioner<ControlMidiToFreq, NAudio_DSP::ControlMidiToFreq_> {
	};
}