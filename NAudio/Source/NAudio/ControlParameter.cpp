#include "ControlParameter.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlParameter_::ControlParameter_() :
			name_(""), displayName_(""),
			min_(0.0f), max_(1.0f),
			type_(ControlParameterTypeContinuous),
			isLogarithmic_(false)
		{
		}

		void
		ControlParameter_::setNormalizedValue(float normVal) {
			if(isLogarithmic_) {
				setValue(MapLinToLog(normVal, min_, max_));
			}
			else {
				setValue(MapFloat(normVal, 0.0f, 1.0f, min_, max_, true));
			}
		}

		float
		ControlParameter_::getNormalizedValue() {
			if(isLogarithmic_) {
				return mapLogToLin(value_, min_, max_);
			}
			else {
				return(MapFloat(value_, min_, max_, 0.0f, 1.0f, true));
			}
		}
	}

	std::string
	ControlParameter::getName() {
		return(gen()->getName());
	}

	ControlParameter&
	ControlParameter::name(std::string name) {
		gen()->setName(name);
		return(*this);
	}

	std::string
	ControlParameter::getDisplayName() {
		return(gen()->getDisplayName());
	}

	ControlParameter&
	ControlParameter::displayName(std::string displayName) {
		gen()->setDisplayName(displayName);
		return(*this);
	}

	float
	ControlParameter::getValue() {
		return(gen()->getValue());
	}

	ControlParameter&
	ControlParameter::value(float value) {
		gen()->setValue(value);
		return(*this);
	}

	float
	ControlParameter::getMin() {
		return(gen()->getMin());
	}

	ControlParameter&
	ControlParameter::min(float min) {
		gen()->setMin(min);
		return(*this);
	}

	ControlParameter&
	ControlParameter::max(float max) {
		gen()->setMax(max);
		return(*this);
	}

	float
	ControlParameter::getMax() {
		return(gen()->getMax());
	}

	ControlParameter&
	ControlParameter::parameterType(ControlParameterType type) {
		gen()->setType(type);
		return(*this);
	}

	ControlParameterType
	ControlParameter::getParameterType() {
		return(gen()->getType());
	}

	ControlParameter&
	ControlParameter::logarithmic(bool isLogarithmic) {
		gen()->setIsLogarithmic(isLogarithmic);
		return(*this);
	}

	bool
	ControlParameter::getIsLogarithmic() {
		return(gen()->getIsLogarithmic());
	}

	ControlParameter&
	ControlParameter::setNormalizedValue(float value) {
		gen()->setNormalizedValue(value);
		return(*this);
	}

	float
	ControlParameter::getNormalizedValue() {
		return(gen()->getNormalizedValue());
	}
}