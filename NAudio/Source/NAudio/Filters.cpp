#include "Filters.h"

namespace NAudio {
	namespace NAudio_DSP {
		Filter_::Filter_() :
			cutoff_(FixedValue(20000.0f)), Q_(FixedValue(0.7071f)),
			bypass_(ControlValue(0.0f)),
			bNormalizeGain_(true)
		{
			workspace_.Resize(kSynthesisBlockSize, 1u, 0.0f);
		}

		void
		Filter_::setInput(Generator input) {
			Effect_::setInput(input);
			setIsStereoInput(input.isStereoOutput());
			setIsStereoOutput(input.isStereoOutput());
		}
	}
}