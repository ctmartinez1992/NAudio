#pragma once

#include "RectWave.h"

namespace NAudio {
	//The PWM of a RectWave_ or RectWaveBL_ is 0.5 by default (square).
	//Quick-and-dirty square wave.
	class SquareWave : public TemplatedGenerator<NAudio_DSP::RectWave_> {
	public:
		NAUDIO_MAKE_GEN_SETTERS(SquareWave, freq, setFrequencyGenerator);
	};

	//Bandlimited square wave.
	class SquareWaveBL : public TemplatedGenerator<NAudio_DSP::RectWaveBL_> {
	public:
		NAUDIO_MAKE_GEN_SETTERS(SquareWaveBL, freq, setFreqGen);
	};
}