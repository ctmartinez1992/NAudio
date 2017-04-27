#pragma once

#include "Generator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class Adder_ : public Generator_ {
		protected:
			std::vector<Generator> inputs_;
			NAudioFrames workSpace_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			Adder_();

			void
			input(Generator generator);

			void
			setIsStereoOutput(bool stereo);

			Generator
			getInput(unsigned int index) {
				return(inputs_[index]);
			}

			unsigned int
			numInputs() {
				return((unsigned int)inputs_.size());
			}
		};

		inline void
		Adder_::computeSynthesisBlock(const SynthesisContext_& context) {
			float* framesData = &outputFrames_[0];

			memset(framesData, 0, sizeof(float) * outputFrames_.Size());

			for(size_t j = 0; j < inputs_.size(); ++j) {
				inputs_[j].tick(workSpace_, context);
				outputFrames_ += workSpace_;				//Add each sample in frames to each sample in workspace.
			}
		}
	}

	class Adder : public TemplatedGenerator<NAudio_DSP::Adder_> {
	public:
		Adder
		input(Generator input) {
			gen()->input(input);
			return(*this);
		}

		Generator
		operator[](unsigned int index) {
			return(gen()->getInput(index));
		}

		unsigned long
		numInputs() {
			return(gen()->numInputs());
		}
	};

	static Adder
	operator+(Generator a, Generator b) {
		Adder add;

		add.input(a);
		add.input(b);

		return(add);
	}

	static Adder
	operator+(float a, Generator b) {
		Adder add;

		add.input(FixedValue(a));
		add.input(b);

		return(add);
	}


	static Adder
	operator+(Generator a, float b) {
		Adder add;

		add.input(a);
		add.input(FixedValue(b));

		return(add);
	}

	static Adder
	operator+(Generator a, ControlGenerator b) {
		return(a + FixedValue().setValue(b));
	}

	static Adder
	operator+(ControlGenerator a, Generator b) {
		return(FixedValue().setValue(a) + b);
	}

	static Adder
	operator+(Adder a, ControlGenerator b) {
		return(a + FixedValue().setValue(b));
	}

	static Adder
	operator+(ControlGenerator a, Adder b) {
		return(FixedValue().setValue(a) + b);
	}

	namespace NAudio_DSP {
		class Subtractor_ : public Generator_ {
		protected:
			Generator left_;
			Generator right_;
			NAudioFrames workSpace_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			Subtractor_();

			void
			setLeft(Generator arg);

			void
			setRight(Generator arg);

			void
			setIsStereoOutput(bool stereo);
		};

		inline void
		Subtractor_::computeSynthesisBlock(const SynthesisContext_& context) {
			left_.tick(outputFrames_, context);
			right_.tick(workSpace_, context);

			outputFrames_ -= workSpace_;
		}
	}

	class Subtractor : public TemplatedGenerator<NAudio_DSP::Subtractor_> {
	public:
		NAUDIO_MAKE_GEN_SETTERS(Subtractor, left, setLeft);
		NAUDIO_MAKE_GEN_SETTERS(Subtractor, right, setRight);
	};

	static Subtractor
	operator-(Generator a, Generator b) {
		Subtractor sub;

		sub.left(a);
		sub.right(b);

		return(sub);
	}

	static Subtractor
	operator-(float a, Generator b) {
		Subtractor sub;

		sub.left(FixedValue(a));
		sub.right(b);

		return(sub);
	}

	static Subtractor
	operator-(Generator a, float b) {
		Subtractor sub;

		sub.left(a);
		sub.right(FixedValue(b));

		return(sub);
	}

	static Subtractor
	operator-(Generator a, ControlGenerator b) {
		return(a - FixedValue().setValue(b));
	}

	static Subtractor
	operator-(ControlGenerator a, Generator b) {
		return(FixedValue().setValue(a) - b);
	}

	namespace NAudio_DSP {
		class Multiplier_ : public Generator_ {

		protected:
			std::vector<Generator> inputs_;
			NAudioFrames workSpace_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			Multiplier_();

			void
			input(Generator generator);

			void
			setIsStereoOutput(bool stereo);

			Generator
			getInput(unsigned int index) {
				return(inputs_[index]);
			}

			unsigned int
			numInputs() {
				return((unsigned int)inputs_.size());
			}
		};

		inline void
		Multiplier_::computeSynthesisBlock(const SynthesisContext_& context) {
			memset(&outputFrames_[0], 0, sizeof(float) * outputFrames_.Size());

			//For the first generator, store the value in the block.
			inputs_[0].tick(outputFrames_, context);

			//For additional generators, use the workspace stkframes for tick, and multiply it into the frames argument.
			for(size_t i = 1; i < inputs_.size(); ++i) {
				inputs_[i].tick(workSpace_, context);
				outputFrames_ *= workSpace_;
			}
		}
	}

	class Multiplier : public TemplatedGenerator<NAudio_DSP::Multiplier_> {
	public:
		Multiplier
		input(Generator inputSource) {
			gen()->input(inputSource);
			return(*this);
		}

		Generator
		operator[](unsigned int index) {
			return(gen()->getInput(index));
		}

		unsigned long
		numInputs() {
			return(gen()->numInputs());
		}
	};

	static Multiplier
	operator*(Generator a, Generator b) {
		Multiplier mult;

		mult.input(a);
		mult.input(b);

		return(mult);
	}

	static Multiplier
	operator*(float a, Generator b) {
		return(FixedValue(a) * b);
	}

	static Multiplier
	operator*(Generator a, float b) {
		return(a * FixedValue(b));
	}

	static Multiplier
	operator*(Generator a, ControlGenerator b) {
		return(a * FixedValue().setValue(b));
	}

	static Multiplier
	operator*(ControlGenerator a, Generator b) {
		return(FixedValue().setValue(a) * b);
	}

	static Multiplier
	operator*(Multiplier a, ControlGenerator b) {
		return(a * FixedValue().setValue(b));
	}

	static Multiplier
	operator*(ControlGenerator a, Multiplier b) {
		return(FixedValue().setValue(a) * b);
	}

	namespace NAudio_DSP {
		class Divider_ : public Generator_ {
		protected:
			Generator left_;
			Generator right_;
			NAudioFrames workSpace_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			Divider_();

			void
			setLeft(Generator arg);

			void
			setRight(Generator arg);

			void
			setIsStereoOutput(bool stereo);
		};

		inline void
		Divider_::computeSynthesisBlock(const SynthesisContext_& context) {
			left_.tick(outputFrames_, context);
			right_.tick(workSpace_, context);

			outputFrames_ /= workSpace_;
		}
	}

	class Divider : public TemplatedGenerator<NAudio_DSP::Divider_> {
	public:
		NAUDIO_MAKE_GEN_SETTERS(Divider, left, setLeft);
		NAUDIO_MAKE_GEN_SETTERS(Divider, right, setRight);
	};

	static Divider
	operator/(Generator a, Generator b) {
		Divider div;

		div.left(a);
		div.right(b);

		return(div);
	}

	static Divider
	operator/(float a, Generator b) {
		Divider div;

		div.left(FixedValue(a));
		div.right(b);

		return(div);
	}

	static Divider
	operator/(Generator a, float b) {
		Divider div;

		div.left(a);
		div.right(FixedValue(b));

		return(div);
	}

	static Divider
	operator/(Generator a, ControlGenerator b) {
		return(a / FixedValue().setValue(b));
	}

	static Divider
	operator/(ControlGenerator a, Generator b) {
		return(FixedValue().setValue(a) / b);
	}
}