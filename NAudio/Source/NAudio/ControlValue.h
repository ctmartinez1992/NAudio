#pragma once

#include "ControlGenerator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlValue_ : public ControlGenerator_ {
		protected:
			float value_;
			bool changed_;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlValue_();
			
			inline void
			setValue(float value) {
				value_ = value;
				changed_ = true;
			}
            
			//Get current value directly.
			float
			getValue() {
				return(value_);
			}
		};
    
		inline void
		ControlValue_::computeOutput(const SynthesisContext_& context) {
			output_.triggered =  (changed_ || context.forceNewOutput);
			changed_ = context.forceNewOutput; 
			output_.value = value_;
		}
	}
	
	class ControlValue : public TemplatedControlGenerator<NAudio_DSP::ControlValue_> {
	public:
		ControlValue(float value = 0.0f) {
			gen()->setValue(value);
		}
		
		ControlValue&
		value(float value) {
			gen()->setValue(value);
			return(*this);
		}
		
		//For higher-level access, doesn't affect tick state.
		float
		getValue() {
			return(gen()->getValue());
		}
	};
}