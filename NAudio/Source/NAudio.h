#pragma once

//Core
	#include "NAudio/NAudioCore.h"
	#include "NAudio/NAudioFrames.h"
	#include "NAudio/SampleTable.h"
	#include "NAudio/FixedValue.h"
	#include "NAudio/Arithmetic.h"
	#include "NAudio/ControlValue.h"
	#include "NAudio/ControlTrigger.h"
	#include "NAudio/ControlParameter.h"
	#include "NAudio/ControlArithmetic.h"
	#include "NAudio/ControlComparison.h"
	#include "NAudio/MonoToStereoPanner.h"
	#include "NAudio/RampedValue.h"
	#include "NAudio/Synth.h"
	#include "NAudio/Mixer.h"

//Generators
	//Oscillators
		#include "NAudio/TableLookupOsc.h"
		#include "NAudio/SineWave.h"

		#include "NAudio/SawtoothWave.h"	//Aliasing
		#include "NAudio/TriangleWave.h"	//Aliasing
		#include "NAudio/SquareWave.h"		//Aliasing
		#include "NAudio/RectWave.h"		//Aliasing

		#include "NAudio/Noise.h"

	//Effects
		#include "NAudio/CombFilter.h"
		#include "NAudio/Filters.h"
		#include "NAudio/StereoDelay.h"
		#include "NAudio/BasicDelay.h"
		#include "NAudio/Reverb.h"
		#include "NAudio/FilterUtils.h"
		#include "NAudio/DelayUtils.h"
		#include "NAudio/Reverb.h"
		#include "NAudio/BitCrusher.h"

	//Utilities
		#include "NAudio/ADSR.h"
		#include "NAudio/RingBuffer.h"
		#include "NAudio/LFNoise.h"

	//Non-Oscillator Audio Sources
		#include "NAudio/BufferPlayer.h"

//Control Generators
	#include "NAudio/ControlDelay.h"
	#include "NAudio/ControlDbToLinear.h"
	#include "NAudio/ControlFloor.h"
	#include "NAudio/ControlPulse.h"
	#include "NAudio/ControlPrinter.h"
	#include "NAudio/ControlXYSpeed.h"
	#include "NAudio/ControlMetro.h"
	#include "NAudio/ControlMetroDivider.h"
	#include "NAudio/ControlSwitcher.h"
	#include "NAudio/ControlCounter.h"
	#include "NAudio/ControlStepper.h"
	#include "NAudio/ControlRandom.h"
	#include "NAudio/ControlSnapToScale.h"
	#include "NAudio/ControlMidiToFreq.h"
	#include "NAudio/ControlTriggerFilter.h"
	#include "NAudio/ControlCallback.h"			//C++11 only

//Util
	#include "NAudio/AudioFileUtils.h"