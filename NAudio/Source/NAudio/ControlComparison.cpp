#include "ControlComparison.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlComparisonOperator_::ControlComparisonOperator_() :
			lhsGen_(ControlValue(0)),
			rhsGen_(ControlValue(0))
		{
		}
	}
}