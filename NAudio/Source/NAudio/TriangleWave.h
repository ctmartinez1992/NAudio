#pragma once

#include "SawtoothWave.h"

namespace NAudio {
	//Quick and dirty triangle wave.
	class TriangleWave : public TemplatedGenerator<NAudio_DSP::AngularWave_> {
	public:
		TriangleWave() {
			gen()->setSlopeGenerator(FixedValue(0.5f));
		}

		NAUDIO_MAKE_GEN_SETTERS(TriangleWave, freq, setFrequencyGenerator);

		//Set from 0-1 to change slope. At 0, it's a falling saw, at 1, it's a rising saw (defaults to 0.5f, triangle).
		NAUDIO_MAKE_GEN_SETTERS(TriangleWave, slope, setSlopeGenerator);
	};
}