#pragma once

#include <iostream>

#include "Effect.h"
#include "ControlGenerator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class BitCrusher_ : public Effect_ {
		protected:
			ControlGenerator bitDepth;

			void
			ComputeSynthesisBlock(const SynthesisContext_& context);

		public:
			BitCrusher_();

			void
			SetBitDepth(ControlGenerator);

			void
			SetInput(Generator input);

			void
			SetIsStereoInput(bool stereo);
		};

		inline void
		BitCrusher_::ComputeSynthesisBlock(const SynthesisContext_& context) {
			float* synthBlockWriteHead = &outputFrames_[0];
			float* dryFramesReadHead = &dryFrames_[0];

			unsigned int nSamples = (unsigned int)outputFrames_.Size();
			float bitDepthValue = Clamp(bitDepth.tick(context).value, 0.0f, 16.0f);
			float bitDepthMax = (float)pow(2, bitDepthValue);

			while(nSamples--) {
				float dry = *(dryFramesReadHead++);
				int snapped = (int)(bitDepthMax * dry);
				float snappedFloat = (float)snapped / (float)bitDepthMax;
				*(synthBlockWriteHead++) = snappedFloat;
			}
		}
	}

	class BitCrusher : public TemplatedEffect<BitCrusher, NAudio_DSP::BitCrusher_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(BitCrusher, bitDepth, SetBitDepth);
	};
}