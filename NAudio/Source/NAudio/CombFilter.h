#pragma once

#include "Effect.h"
#include "DelayUtils.h"
#include "FilterUtils.h"

namespace NAudio {
	namespace NAudio_DSP {
		//Absrtract comb-filter gen_ base class.
		class CombFilter_ : public Effect_ {
		protected:
			DelayLine delayLine_;
			Generator delayTimeGen_;
			ControlGenerator scaleFactorCtrlGen_;
			NAudioFrames delayTimeFrames_;

		public:
			CombFilter_();

			void
			initialize(float initialDelayTime, float maxDelayTime);

			void
			setDelayTimeGen(Generator gen) {
				delayTimeGen_ = gen;
			}

			void
			setScaleFactorGen(ControlGenerator gen) {
				scaleFactorCtrlGen_ = gen;
			}
		};

		//Basic mono feed-forward comb filter.
		class FFCombFilter_ : public CombFilter_ {
		protected:
			inline void
			computeSynthesisBlock(const SynthesisContext_& context) {
				//Tick modulations.
				delayTimeGen_.tick(delayTimeFrames_, context);

				float* inptr = &dryFrames_[0];
				float* outptr = &outputFrames_[0];
				float* dtptr = &delayTimeFrames_[0];

				float sf = scaleFactorCtrlGen_.tick(context).value;

				float norm = (1.0f / (1.0f + sf));

				for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
					delayLine_.tickIn(*inptr);
					*outptr++ = (*inptr++ + delayLine_.tickOut(*dtptr++) * sf) * norm;
					delayLine_.advance();
				}
			}
		};

		//Basic mono feedback comb filter with hook for aditional processing pre-feedback.
		class FBCombFilter_ : public CombFilter_ {
		protected:
			inline void
			computeSynthesisBlock(const SynthesisContext_& context) {
				//Tick modulations.
				delayTimeGen_.tick(delayTimeFrames_, context);

				float* inptr = &dryFrames_[0];
				float* outptr = &outputFrames_[0];
				float* dtptr = &delayTimeFrames_[0];

				float y = 0;
				float sf = scaleFactorCtrlGen_.tick(context).value;
				float norm = (1.0f / (1.0f + sf));

				for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
					y = ((delayLine_.tickOut(*dtptr++) * sf) + *inptr++) * norm;
					delayLine_.tickIn(y);
					*outptr++ = y;
					delayLine_.advance();
				}
			}
		};

		//Feedback comb filter with 6dB/oct (one-pole) lpf and hpf, optimized for reverb.
		class FilteredFBCombFilter6_ : public CombFilter_ {
		protected:
			float lastOutLow_;
			float lastOutHigh_;

			ControlGenerator lowCutoffGen_;
			ControlGenerator highCutoffGen_;
			ControlGenerator scaleFactorGen_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			FilteredFBCombFilter6_();

			void
			setLowCutoff(ControlGenerator gen) {
				lowCutoffGen_ = gen;
			}

			void
			setHighCutoff(ControlGenerator gen) {
				highCutoffGen_ = gen;
			}
		};

		inline void
		FilteredFBCombFilter6_::computeSynthesisBlock(const SynthesisContext_& context) {
			//Tick modulations.
			delayTimeGen_.tick(delayTimeFrames_, context);

			float* inptr = &dryFrames_[0];
			float* outptr = &outputFrames_[0];
			float* dtptr = &delayTimeFrames_[0];

			float y = 0;
			float sf = scaleFactorCtrlGen_.tick(context).value;

			float lowCoef = cutoffToOnePoleCoef(lowCutoffGen_.tick(context).value);
			float hiCoef = 1.0f - cutoffToOnePoleCoef(highCutoffGen_.tick(context).value);

			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				onePoleLPFTick(delayLine_.tickOut(*dtptr++), lastOutLow_, lowCoef);
				onePoleHPFTick(lastOutLow_, lastOutHigh_, hiCoef);

				//No normalization on purpose.
				y = ((lastOutHigh_ * sf) + *inptr++);

				delayLine_.tickIn(y);
				*outptr++ = y;
				delayLine_.advance();
			}
		}
	}

	class FFCombFilter : public TemplatedEffect<FFCombFilter, NAudio_DSP::FFCombFilter_> {
	public:
		FFCombFilter(float initialDelayTime = 0.1f, float maxDelayTime = -1.0f);

		NAUDIO_MAKE_GEN_SETTERS(FFCombFilter, delayTime, setDelayTimeGen);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(FFCombFilter, scaleFactor, setScaleFactorGen);
	};

	class FBCombFilter : public TemplatedEffect<FBCombFilter, NAudio_DSP::FBCombFilter_> {
	public:
		FBCombFilter(float initialDelayTime = 0.1f, float maxDelayTime = -1.0f);

		NAUDIO_MAKE_GEN_SETTERS(FBCombFilter, delayTime, setDelayTimeGen);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(FBCombFilter, scaleFactor, setScaleFactorGen);
	};

	class FilteredFBCombFilter6 : public TemplatedEffect<FilteredFBCombFilter6, NAudio_DSP::FilteredFBCombFilter6_> {
	public:
		FilteredFBCombFilter6(float initialDelayTime = 0.1f, float maxDelayTime = -1.0f);

		NAUDIO_MAKE_GEN_SETTERS(FilteredFBCombFilter6, delayTime, setDelayTimeGen);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(FilteredFBCombFilter6, scaleFactor, setScaleFactorGen);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(FilteredFBCombFilter6, lowpassCutoff, setLowCutoff);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(FilteredFBCombFilter6, highpassCutoff, setHighCutoff);
	};
}