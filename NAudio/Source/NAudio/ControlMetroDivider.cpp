#include "ControlMetroDivider.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlMetroDivider_::ControlMetroDivider_() :
			tickCounter_(0u)
		{
			divisonsGen_ = ControlValue(1.0f);
			offsetGen_ = ControlValue(0.0f);

			output_.value = 1.0f;
		}
	}
}