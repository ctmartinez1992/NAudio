#pragma once

#include "ControlGenerator.h"
#include "ControlValue.h"

using std::vector;

namespace NAudio {
	//Adder
	namespace NAudio_DSP {
		class ControlAdder_ : public ControlGenerator_ {
		public:
			void
			input(ControlGenerator input);
      
			ControlGenerator
			getInput(unsigned int index) {
				return(inputs[index]);
			}
			
			unsigned long
			numInputs() {
				return((unsigned long)inputs.size());
			}

		protected:
			vector<ControlGenerator> inputs;

			void
			computeOutput(const SynthesisContext_& context);
		};
    
		inline void
		ControlAdder_::computeOutput(const SynthesisContext_& context) {
			output_.triggered = false;

			for(unsigned int i = 0; i < inputs.size(); ++i) {
				if(inputs[i].tick(context).triggered) {
					output_.triggered = true;
					break;
				}
			}

			float sum = 0.0f;
			
			for(unsigned int i = 0; i < inputs.size(); ++i) {
				sum += inputs[i].tick(context).value;
			}
			
			output_.value = sum;
		}
	}

	class ControlAdder : public TemplatedControlGenerator<NAudio_DSP::ControlAdder_> {
	public:
		void
		input(ControlGenerator input) {
			gen()->input(input);
		}
    
		ControlGenerator
		operator[](unsigned int index) {
			return(gen()->getInput(index));
		}
		
		unsigned long
		numInputs() {
			return(gen()->numInputs());
		}
	};
  
	static ControlAdder
	operator+(ControlGenerator a, ControlGenerator b) {
		ControlAdder adder;
		adder.input(a);
		adder.input(b);
		return(adder);
	}
	
	static ControlAdder
	operator+(ControlGenerator a, float b) {
		ControlAdder adder;
		adder.input(a);
		adder.input(ControlValue(b));
		return(adder);
	}
	
	static ControlAdder
	operator+(float a, ControlGenerator b) {
		ControlAdder adder;
		adder.input(ControlValue(a));
		adder.input(ControlGenerator(b));
		return(adder);
	}

	//Subtractor
	namespace NAudio_DSP {
		class ControlSubtractor_ : public ControlGenerator_ {
		protected:
			ControlGenerator left;
			ControlGenerator right;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			void
			setLeft(ControlGenerator arg) {
				left = arg;
			}

			void
			setRight(ControlGenerator arg) {
				right = arg;
			}
		};

		inline void
		ControlSubtractor_::computeOutput(const SynthesisContext_& context) {
			ControlGeneratorOutput leftOut = left.tick(context);
			ControlGeneratorOutput rightOut = right.tick(context);

			if(!leftOut.triggered && !rightOut.triggered) {
				output_.triggered = false;
			}
			else {
				output_.triggered = true;
				output_.value = leftOut.value - rightOut.value;
			}
		}
	}

	class ControlSubtractor : public TemplatedControlGenerator<NAudio_DSP::ControlSubtractor_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlSubtractor, left, setLeft);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlSubtractor, right, setRight);
	};

	static ControlSubtractor
	operator-(ControlGenerator a, ControlGenerator b) {
		ControlSubtractor subtractor;
		subtractor.left(a);
		subtractor.right(b);
		return(subtractor);
	}

	static ControlSubtractor
	operator-(ControlGenerator a, float b) {
		ControlSubtractor subtractor;
		subtractor.left(a);
		subtractor.right(ControlValue(b));
		return(subtractor);
	}

	static ControlSubtractor
	operator-(float a, ControlGenerator b) {
		ControlSubtractor subtractor;
		subtractor.left(ControlValue(a));
		subtractor.right(ControlGenerator(b));
		return(subtractor);
	}

	//Multiplier
	namespace NAudio_DSP {
		class ControlMultiplier_ : public ControlGenerator_ {
		public:
			void
			input(ControlGenerator input);

			ControlGenerator
			getInput(unsigned int index) {
				return(inputs[index]);
			}

			unsigned long
			numInputs() {
				return((unsigned long)inputs.size());
			}

		protected:
			vector<ControlGenerator> inputs;

			void
			computeOutput(const SynthesisContext_& context);
		};

		inline void
		ControlMultiplier_::computeOutput(const SynthesisContext_& context) {
			output_.triggered = false;

			for(unsigned int i = 0; i < inputs.size(); ++i) {
				if(inputs[i].tick(context).triggered) {
					output_.triggered = true;
					break;
				}
			}

			float product = inputs[0].tick(context).value;

			for(unsigned int i = 1; i < inputs.size(); ++i) {
				product *= inputs[i].tick(context).value;
			}

			output_.value = product;
		}
	}

	class ControlMultiplier : public TemplatedControlGenerator<NAudio_DSP::ControlMultiplier_> {
	public:
		void
		input(ControlGenerator input) {
			gen()->input(input);
		}

		ControlGenerator
		operator[](unsigned int index) {
			return(gen()->getInput(index));
		}

		unsigned long
		numInputs() {
			return(gen()->numInputs());
		}
	};

	static ControlMultiplier
	operator*(ControlGenerator a, ControlGenerator b) {
		ControlMultiplier mult;
		mult.input(a);
		mult.input(b);
		return(mult);
	}

	static ControlMultiplier
	operator*(ControlGenerator a, float b) {
		ControlMultiplier mult;
		mult.input(a);
		mult.input(ControlValue(b));
		return(mult);
	}

	static ControlMultiplier
	operator*(float a, ControlGenerator b) {
		ControlMultiplier mult;
		mult.input(ControlValue(a));
		mult.input(ControlGenerator(b));
		return(mult);
	}

	//Divider
	namespace NAudio_DSP {
		class ControlDivider_ : public ControlGenerator_ {
		protected:
			ControlGenerator left;
			ControlGenerator right;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			void
			setLeft(ControlGenerator arg) {
				left = arg;
			}

			void
			setRight(ControlGenerator arg) {
				right = arg;
			}
		};

		inline void
		ControlDivider_::computeOutput(const SynthesisContext_& context) {
			ControlGeneratorOutput leftOut = left.tick(context);
			ControlGeneratorOutput rightOut = right.tick(context);

			bool rightIsZero = (rightOut.value == 0.0f);

			if(rightIsZero) {
				LOG(NLOG_ERROR, "ControlGenerator divide by zero encountered. Returning last valid value.");
			}

			bool noChange = (!leftOut.triggered && !rightOut.triggered);

			if(rightIsZero || noChange) {
				output_.triggered = false;
			}
			else {
				output_.triggered = true;
				output_.value = leftOut.value / rightOut.value;
			}
		}
	}

	class ControlDivider : public TemplatedControlGenerator<NAudio_DSP::ControlDivider_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlDivider, left, setLeft);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlDivider, right, setRight);
	};

	static ControlDivider
	operator/(ControlGenerator a, ControlGenerator b) {
		ControlDivider divider;
		divider.left(a);
		divider.right(b);
		return(divider);
	}

	static ControlDivider
	operator/(ControlGenerator a, float b) {
		ControlDivider divider;
		divider.left(a);
		divider.right(ControlValue(b));
		return(divider);
	}

	static ControlDivider
	operator/(float a, ControlGenerator b) {
		ControlDivider divider;
		divider.left(ControlValue(a));
		divider.right(ControlGenerator(b));
		return(divider);
	}
}