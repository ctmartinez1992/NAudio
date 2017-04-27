#include "Effect.h"

namespace NAudio {
	namespace NAudio_DSP {
		Effect_::Effect_() :
			isStereoInput_(false)
		{
			dryFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);
			bypassGen_ = ControlValue(0);
		}
    
		WetDryEffect_::WetDryEffect_() {
			mixWorkspace_.Resize(kSynthesisBlockSize, 1u, 0.0f);
			dryLevelGen_ = FixedValue(0.5f);
			wetLevelGen_ = FixedValue(0.5f);
		}
	}
}