#pragma once

#include "ControlConditioner.h"

#if NAUDIO_HAS_CPP_11
#include <functional>
#include "Synth.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlCallback_ : public ControlConditioner_ {
		protected:
			function<void(ControlGeneratorOutput)> callback_;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			void
			setCallback(function<void(ControlGeneratorOutput)> fn);
		};
	}

	//Pass a lambda function to a controlgenerator. That function will be called when the input ControlGenerator sends a "triggered" message.
	//Constructor takes two arguments: The synth that owns the ControlCallback, and a lambda function.

	class ControlCallback : public TemplatedControlConditioner<ControlCallback, NAudio_DSP::ControlCallback_> {
	public:
		ControlCallback(Synth* synth, function<void(ControlGeneratorOutput)>);
	};
}
#endif