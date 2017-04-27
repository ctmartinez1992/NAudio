#pragma once

#include "ControlValue.h"

namespace NAudio {
	//Base Comparison ControlGenerator
	namespace NAudio_DSP {
		class ControlComparisonOperator_ : public ControlGenerator_ {
		protected:
			ControlGenerator lhsGen_;
			ControlGenerator rhsGen_;

			virtual bool
			satisfiesCondition(float l, float r) = 0;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlComparisonOperator_();

			void
			setLeft(ControlGenerator gen) {
				lhsGen_ = gen;
			}
			void
			setRight(ControlGenerator gen) {
				rhsGen_ = gen;
			}
		};

		inline void ControlComparisonOperator_::computeOutput(const SynthesisContext_& context) {
			output_.triggered = false;

			ControlGeneratorOutput lhsOut = lhsGen_.tick(context);
			ControlGeneratorOutput rhsOut = rhsGen_.tick(context);

			if(lhsOut.triggered) {
				output_.value = satisfiesCondition(lhsOut.value, rhsOut.value) ? 1.0f : 0.0f;
				output_.triggered = true;
			}
			else {
				output_.value = 0.0f;
			}
		}
	}

	template <class OperatorType>
	class TemplatedControlComparisonOperator : public TemplatedControlGenerator<OperatorType> {
	public:
		void
		setLeft(ControlGenerator gen) {
			this->gen()->setLeft(gen);
		}

		void
		setRight(ControlGenerator gen) {
			this->gen()->setRight(gen);
		}
	};

	//Equality
	namespace NAudio_DSP {
		class ControlEquals_ : public ControlComparisonOperator_ {
		protected:
			inline bool
			satisfiesCondition(float l, float r) {
				return(l == r);
			}
		};
	}

	class ControlEquals : public TemplatedControlComparisonOperator<NAudio_DSP::ControlEquals_> {
	};

	static ControlEquals
	operator==(ControlGenerator lhs, ControlGenerator rhs) {
		ControlEquals c;

		c.setLeft(lhs);
		c.setRight(rhs);

		return(c);
	}

	static ControlEquals
	operator==(ControlGenerator lhs, float right) {
		ControlEquals c;

		c.setLeft(lhs);
		c.setRight(ControlValue(right));

		return(c);
	}

	//Not Equals
	namespace NAudio_DSP {
		class ControlNotEquals_ : public ControlComparisonOperator_ {
		protected:
			inline bool
			satisfiesCondition(float l, float r) {
				return(l != r);
			}
		};
	}

	class ControlNotEquals : public TemplatedControlComparisonOperator<NAudio_DSP::ControlNotEquals_> {
	};

	static ControlNotEquals
	operator!=(ControlGenerator lhs, ControlGenerator rhs) {
		ControlNotEquals c;

		c.setLeft(lhs);
		c.setRight(rhs);

		return(c);
	}

	static ControlNotEquals
	operator!=(ControlGenerator lhs, float right) {
		ControlNotEquals c;

		c.setLeft(lhs);
		c.setRight(ControlValue(right));

		return(c);
	}

	//Greater Than
	namespace NAudio_DSP {
		class ControlGreaterThan_ : public ControlComparisonOperator_ {
		protected:
			inline bool
			satisfiesCondition(float l, float r) {
				return(l > r);
			}
		};
	}

	class ControlGreaterThan : public TemplatedControlComparisonOperator<NAudio_DSP::ControlGreaterThan_> {
	};

	static ControlGreaterThan
	operator>(ControlGenerator lhs, ControlGenerator rhs) {
		ControlGreaterThan c;

		c.setLeft(lhs);
		c.setRight(rhs);

		return(c);
	}

	static ControlGreaterThan
	operator>(ControlGenerator lhs, float right) {
		ControlGreaterThan c;

		c.setLeft(lhs);
		c.setRight(ControlValue(right));

		return(c);
	}

	//Greater Or Equal
	namespace NAudio_DSP {
		class ControlGreaterOrEqual_ : public ControlComparisonOperator_ {
		protected:
			inline bool
			satisfiesCondition(float l, float r) {
				return(l >= r);
			}
		};
	}

	class ControlGreaterOrEqual : public TemplatedControlComparisonOperator<NAudio_DSP::ControlGreaterOrEqual_> {
	};

	static ControlGreaterOrEqual
	operator>=(ControlGenerator lhs, ControlGenerator rhs) {
		ControlGreaterOrEqual c;

		c.setLeft(lhs);
		c.setRight(rhs);

		return(c);
	}

	static ControlGreaterOrEqual
	operator>=(ControlGenerator lhs, float right) {
		ControlGreaterOrEqual c;

		c.setLeft(lhs);
		c.setRight(ControlValue(right));

		return(c);
	}

	//Less Than
	namespace NAudio_DSP {
		class ControlLessThan_ : public ControlComparisonOperator_ {
		protected:
			inline bool
			satisfiesCondition(float l, float r) {
				return(l < r);
			}
		};
	}

	class ControlLessThan : public TemplatedControlComparisonOperator<NAudio_DSP::ControlLessThan_> {
	};

	static ControlLessThan
	operator<(ControlGenerator lhs, ControlGenerator rhs) {
		ControlLessThan c;

		c.setLeft(lhs);
		c.setRight(rhs);

		return(c);
	}

	static ControlLessThan
	operator<(ControlGenerator lhs, float right) {
		ControlLessThan c;

		c.setLeft(lhs);
		c.setRight(ControlValue(right));

		return(c);
	}

	//Less Or Equal
	namespace NAudio_DSP {
		class ControlLessOrEqual_ : public ControlComparisonOperator_ {
		protected:
			inline bool
			satisfiesCondition(float l, float r) {
				return(l <= r);
			}
		};
	}

	class ControlLessOrEqual : public TemplatedControlComparisonOperator<NAudio_DSP::ControlLessOrEqual_> {
	};

	static ControlLessOrEqual
	operator<=(ControlGenerator lhs, ControlGenerator rhs) {
		ControlLessOrEqual c;

		c.setLeft(lhs);
		c.setRight(rhs);

		return(c);
	}

	static ControlLessOrEqual
	operator<=(ControlGenerator lhs, float right) {
		ControlLessOrEqual c;

		c.setLeft(lhs);
		c.setRight(ControlValue(right));

		return(c);
	}
}