#include "RampedValue.h"

namespace NAudio {
	namespace NAudio_DSP {
		RampedValue_::RampedValue_() :
			target_(0.0f),
			last_(0.0f),
			inc_(0.0f),
			count_(0ul),
			len_(0ul),
			finished_(true)
		{
		}

		RampedValue_::~RampedValue_() {
		}

		bool
		RampedValue_::isFinished() {
			return(finished_);
		}
	}

	bool
	RampedValue::isFinished() {
		return(gen()->isFinished());
	}
}