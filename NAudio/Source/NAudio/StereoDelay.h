#pragma once

#include "Effect.h"
#include "DelayUtils.h"

namespace NAudio {
	namespace NAudio_DSP {
		class StereoDelay_ : public WetDryEffect_ {
		protected:
			Generator delayTimeGen_[2];
			NAudioFrames delayTimeFrames_[2];

			Generator fbkGen_;
			NAudioFrames fbkFrames_;

			DelayLine delayLine_[2];

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			StereoDelay_();

			void
			initialize(float leftDelayArg, float rightDelayArg, float maxDelayLeft, float maxDelayRight);

			void
			setFeedback(Generator arg) {
				fbkGen_ = arg;
			}

			void
			setDelayTimeLeft(Generator arg) {
				delayTimeGen_[0] = arg;
			}

			void
			setDelayTimeRight(Generator arg) {
				delayTimeGen_[1] = arg;
			}
		};

		inline void
		StereoDelay_::computeSynthesisBlock(const SynthesisContext_& context) {
			delayTimeGen_[0].tick(delayTimeFrames_[NAUDIO_LEFT], context);
			delayTimeGen_[1].tick(delayTimeFrames_[NAUDIO_RIGHT], context);

			fbkGen_.tick(fbkFrames_, context);

			float outSamp[2];
			float fbk;

			float *dryptr = &dryFrames_[0];
			float *outptr = &outputFrames_[0];
			float *fbkptr = &fbkFrames_[0];
			float *delptr_l = &(delayTimeFrames_[NAUDIO_LEFT])[0];
			float *delptr_r = &(delayTimeFrames_[NAUDIO_RIGHT])[0];

			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				//Don't clamp feedback,be careful! Negative feedback could be interesting.
				fbk = *fbkptr++;

				outSamp[NAUDIO_LEFT] = delayLine_[NAUDIO_LEFT].tickOut(*delptr_l++);
				outSamp[NAUDIO_RIGHT] = delayLine_[NAUDIO_RIGHT].tickOut(*delptr_r++);

				//Output left sample.
				*outptr++ = outSamp[NAUDIO_LEFT];
				delayLine_[NAUDIO_LEFT].tickIn(*dryptr++ + outSamp[NAUDIO_LEFT] * fbk);

				//Output right sample.
				*outptr++ = outSamp[NAUDIO_RIGHT];
				delayLine_[NAUDIO_RIGHT].tickIn(*dryptr++ + outSamp[NAUDIO_RIGHT] * fbk);

				//Advance delay lines.
				delayLine_[NAUDIO_LEFT].advance();
				delayLine_[NAUDIO_RIGHT].advance();
			}
		}
	}
	
	class StereoDelay : public TemplatedWetDryEffect<StereoDelay, NAudio_DSP::StereoDelay_> {
	public:
		StereoDelay(float leftDelay, float rightDelay, float maxDelayLeft = -1.0f, float maxDelayRight = -1.0f);

		NAUDIO_MAKE_GEN_SETTERS(StereoDelay, feedback, setFeedback)
		NAUDIO_MAKE_GEN_SETTERS(StereoDelay, delayTimeLeft, setDelayTimeLeft)
		NAUDIO_MAKE_GEN_SETTERS(StereoDelay, delayTimeRight, setDelayTimeRight)
	};
}