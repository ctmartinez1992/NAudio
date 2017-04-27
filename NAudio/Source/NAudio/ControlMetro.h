#pragma once

#include "ControlGenerator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlMetro_ : public ControlGenerator_ {
		protected:
			double lastClickTime_;

			ControlGenerator bpm_;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlMetro_();

			void
			setBPMGen(ControlGenerator bpmGen) {
				bpm_ = bpmGen;
			}
		};

		inline void
		ControlMetro_::computeOutput(const SynthesisContext_& context) {
			double sPerBeat = 60.0 / (double)Max(0.001f, bpm_.tick(context).value);
			double delta = context.elapsedTime - lastClickTime_;

			if(delta >= 2.0 * sPerBeat || delta < 0.0) {
				//Account for bpm interval outrunning tick interval or timer wrap-around
				lastClickTime_ = context.elapsedTime;
				output_.triggered = true;
			}
			else if(delta >= sPerBeat) {
				//Acocunt for drift
				lastClickTime_ += sPerBeat;
				output_.triggered = true;
			}
			else {
				output_.triggered = false;
			}

			output_.value = 1.0f;
		}
	}

	//Ouputs a "changed" status at a regular BPM interval.
	class ControlMetro : public TemplatedControlGenerator<NAudio_DSP::ControlMetro_> {
	public:
		ControlMetro(float bpm = 120.0f) {
			gen()->setBPMGen(ControlValue(bpm));
		}

		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlMetro, bpm, setBPMGen);
	};
}