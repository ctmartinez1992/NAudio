#pragma once

#include "ControlValue.h"
#include "Generator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class FixedValue_ : public Generator_ {
		protected:
			ControlGenerator valueGen;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			FixedValue_(float f = 0.0f);

			void
			setValue(ControlGenerator val) {
				valueGen = val;
			}
		};

		inline void
		FixedValue_::computeSynthesisBlock(const SynthesisContext_& context) {
			float* buffStart = &outputFrames_[0];

			ControlGeneratorOutput valueOutput = valueGen.tick(context);

			if(valueOutput.triggered) {
				std::fill(buffStart, buffStart + outputFrames_.Size(), valueOutput.value);
			}
		}
	}

	class FixedValue : public TemplatedGenerator<NAudio_DSP::FixedValue_> {
	public:
		FixedValue() {
		}

		FixedValue(float val) {
			setValue(val);
		}

		FixedValue&
		setValue(float val) {
			gen()->setValue(ControlValue(val));
			return(*this);
		}

		FixedValue&
		setValue(ControlGenerator val) {
			gen()->setValue(val);
			return(*this);
		}
	};
}