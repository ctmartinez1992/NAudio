#pragma once

#include "ControlValue.h"

namespace NAudio {
	typedef enum {
		ControlParameterTypeContinuous = 0,
		ControlParameterTypeToggle,
		ControlParameterTypeMomentary
	} ControlParameterType;

	namespace NAudio_DSP {
		//Parameter for controlling a Synth instance.
		//ControlParameter acts like a ControlValue but provides an interface for exposing itself to a synth, including min, max, type, etc.
		class ControlParameter_ : public ControlValue_ {
		protected:
			std::string					name_;
			std::string					displayName_;

			float						min_;
			float						max_;

			ControlParameterType		type_;

			bool						isLogarithmic_;

		public:
			ControlParameter_();

			void
			setName(std::string name) {
				name_ = name;
			}

			std::string
			getName() {
				return(name_);
			}

			void
			setDisplayName(std::string displayName) {
				displayName_ = displayName;
			}

			std::string
			getDisplayName() {
				return(displayName_);
			}

			void
			setMin(float min) {
				min_ = min;
			}

			float
			getMin() {
				return(min_);
			}

			void
			setMax(float max) {
				max_ = max;
			}

			float
			getMax() {
				return(max_);
			}

			void
			setType(ControlParameterType type) {
				type_ = type;
			}

			ControlParameterType
			getType() {
				return(type_);
			}

			void
			setIsLogarithmic(bool isLogarithmic) {
				isLogarithmic_ = isLogarithmic;
			}

			bool
			getIsLogarithmic() {
				return(isLogarithmic_);
			}

			void
			setNormalizedValue(float normVal);

			float
			getNormalizedValue();
		};
	}

	class ControlParameter : public TemplatedControlGenerator<NAudio_DSP::ControlParameter_> {
	public:
		std::string
		getName();

		ControlParameter&
		name(std::string name);

		std::string
		getDisplayName();

		ControlParameter&
		displayName(std::string displayName);

		float
		getValue();

		ControlParameter&
		value(float value);

		float
		getMin();

		ControlParameter&
		min(float min);

		float
		getMax();

		ControlParameter&
		max(float max);

		ControlParameterType
		getParameterType();

		ControlParameter&
		parameterType(ControlParameterType type);

		bool
		getIsLogarithmic();

		ControlParameter&
		logarithmic(bool isLogarithmic);

		//Convenience methods for setting/getting value normalized linearly 0-1, mapped to min/max range, with log applied if necessary.
		float
		getNormalizedValue();

		ControlParameter&
		setNormalizedValue(float value);
	};
}