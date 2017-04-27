#pragma once

#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlFloor_ : public ControlConditioner_ {
			inline void
			computeOutput(const SynthesisContext_& context) {
				output_.value = (int)(input_.tick(context).value);
				output_.triggered = input_.tick(context).triggered;
			}
		};
	}

	class ControlFloor : public TemplatedControlConditioner<ControlFloor, NAudio_DSP::ControlFloor_> {
	};
}