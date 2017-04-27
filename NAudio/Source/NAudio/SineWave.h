#pragma once

#include "TableLookupOsc.h"

namespace NAudio {
	class SineWave : public TemplatedGenerator<NAudio_DSP::TableLookupOsc_> {
	public:
		SineWave();

		NAUDIO_MAKE_GEN_SETTERS(SineWave, freq, setFrequency)
	};
}