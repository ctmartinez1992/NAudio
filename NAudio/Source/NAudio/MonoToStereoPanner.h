#pragma once

#include "Effect.h"
#include "ControlGenerator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class MonoToStereoPanner_ : public Effect_ {
		protected:
			ControlGenerator panControlGen;
			NAudioFrames panFrames;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			MonoToStereoPanner_();
			~MonoToStereoPanner_();

			void
			setPan(ControlGenerator);
		};

		inline void
		MonoToStereoPanner_::computeSynthesisBlock(const SynthesisContext_& context) {
			float* synthBlockWriteHead = &outputFrames_[0];
			float* dryFramesReadHead = &dryFrames_[0];

			unsigned int nSamples = kSynthesisBlockSize;

			float panValue = panControlGen.tick(context).value;
			float leftVol = 1.0f - Max(0.0f, panValue);
			float rightVol = 1.0f + Min(0.0f, panValue);

			while(nSamples--) {
				*(synthBlockWriteHead++) = *dryFramesReadHead * leftVol;
				*(synthBlockWriteHead++) = *dryFramesReadHead++ * rightVol;
			}
		}
	}

	class MonoToStereoPanner : public TemplatedEffect<MonoToStereoPanner, NAudio_DSP::MonoToStereoPanner_> {
	public:
		//Pan is a -1 to 1 value.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(MonoToStereoPanner, pan, setPan);
	};
}