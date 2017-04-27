#pragma once

#include <vector>

#include "ControlGenerator.h"
#include "ControlValue.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlConditioner_ : public ControlGenerator_ {
		protected:
			ControlGenerator input_;

		public:
			void
			input(ControlGenerator input);
		};
	}

	//The NAudio:: component of the ControlConditioner (the smart pointer wrapper) must subclass the following template, in the form <class Wrapper, class PrivateControlGenerator>
	template<class ControlConditionerType, class ControlConditionerType_>
	class TemplatedControlConditioner : public TemplatedControlGenerator<ControlConditionerType_> {
	public:
		ControlConditionerType
		input(ControlGenerator input) {
			//WHAT!? Why doesn't this compile without the this keyword?
			this->gen()->input(input);
			return(static_cast<ControlConditionerType&>(*this));
		}

		ControlConditionerType
		input(float inputVal) {
			return(input(ControlValue(inputVal)));
		}
	};

	//Signal flow operator. Sets lhs as input to rhs.
	template<class ControlConditionerType, class ControlConditionerType_>
	static ControlConditionerType
	operator>>(ControlGenerator lhs, TemplatedControlConditioner<ControlConditionerType, ControlConditionerType_> rhs) {
		return(rhs.input(lhs));
	}
}