#pragma once

#include "ControlGenerator.h"
#include "RampedValue.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlGenerator_::ControlGenerator_() :
			lastFrameIndex_(0)
		{
		}
		
		ControlGenerator_::~ControlGenerator_() {
		}
		
		ControlGeneratorOutput
		ControlGenerator_::initialOutput() {
			//Rturn output from passing in dummy context.
			return(tick(NAudio::DummyContext));
		}
	}
	
	RampedValue
	ControlGenerator::smoothed(float length) {
		return(RampedValue(obj->initialOutput().value, length).target(*this));
	}
}