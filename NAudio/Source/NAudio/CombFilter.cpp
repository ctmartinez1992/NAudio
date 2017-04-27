#include "CombFilter.h"

namespace NAudio {
	namespace NAudio_DSP {

		CombFilter_::CombFilter_() {
			delayTimeFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);
		}

		void
		CombFilter_::initialize(float initialDelayTime, float maxDelayTime) {
			if(maxDelayTime < 0.0f) {
				maxDelayTime = initialDelayTime * 1.5f;
			}

			delayLine_.initialize(maxDelayTime, 1u);
			delayTimeGen_ = FixedValue(initialDelayTime);
		}

		FilteredFBCombFilter6_::FilteredFBCombFilter6_() :
			lastOutLow_(0.0f), lastOutHigh_(0.0f)
		{
			//Don't care about interpolation here, since this is optimized for reverb (faster).
			delayLine_.setInterpolates(false);
		}
	}

	FFCombFilter::FFCombFilter(float initialDelayTime, float maxDelayTime) {
		gen()->initialize(initialDelayTime, maxDelayTime);
		delayTime(initialDelayTime);
		scaleFactor(0.5f);
	}

	FBCombFilter::FBCombFilter(float initialDelayTime, float maxDelayTime) {
		gen()->initialize(initialDelayTime, maxDelayTime);
		delayTime(initialDelayTime);
		scaleFactor(0.5f);
	}

	FilteredFBCombFilter6::FilteredFBCombFilter6(float initialDelayTime, float maxDelayTime) {
		gen()->initialize(initialDelayTime, maxDelayTime);
		delayTime(initialDelayTime);
		scaleFactor(0.5f);
		lowpassCutoff(12000.0f);
		highpassCutoff(20.0f);
	}
}