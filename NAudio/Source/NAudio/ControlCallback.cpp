#include "ControlCallback.h"

#if NAUDIO_HAS_CPP_11
namespace NAudio {
	namespace NAudio_DSP {
		void
		ControlCallback_::computeOutput(const SynthesisContext_& context) {
			ControlGeneratorOutput inputOut = input_.tick(context);
			
			if(inputOut.triggered) {
				callback_(inputOut);
			}
		}
		
		void
		ControlCallback_::setCallback(function<void(ControlGeneratorOutput)> fn) {
			callback_ = fn;
		}
	}

	ControlCallback::ControlCallback(Synth* synth, function<void(ControlGeneratorOutput)> fn) {
		synth->addAuxControlGenerator(*this);
		gen()->setCallback(fn);
	}
}
#endif