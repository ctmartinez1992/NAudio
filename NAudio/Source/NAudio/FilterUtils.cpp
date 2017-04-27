#include "FilterUtils.h"

namespace NAudio {

	Biquad::Biquad() {
		memset(coef_, 0, 5 * sizeof(float));

		inputVec_.Resize(kSynthesisBlockSize + 4, 1u, 0.0f);
		outputVec_.Resize(kSynthesisBlockSize + 4, 1u, 0.0f);
	}
}