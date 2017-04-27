#pragma once

#include "Effect.h"
#include "DelayUtils.h"
#include "FilterUtils.h"

namespace NAudio {
	namespace NAudio_DSP {
		class Compressor_ : public Effect_ {
		protected:
			//Can be overridden for sidechaining.
			Generator amplitudeInput_;

			ControlGenerator makeupGainGen_;
			ControlGenerator attackGen_;
			ControlGenerator releaseGen_;
			ControlGenerator threshGen_;
			ControlGenerator ratioGen_;
			ControlGenerator lookaheadGen_;

			DelayLine lookaheadDelayLine_;

			NAudioFrames ampInputFrames_;

			float ampEnvValue_;
			float gainEnvValue_;

			bool isLimiter_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			Compressor_();

			//Base class methods overridden here for specialized input behavior.
			void
			setInput(Generator input);

			void
			tick(NAudioFrames& frames, const SynthesisContext_& context);
			void
			tickThrough(NAudioFrames& inFrames, NAudioFrames& outFrames, const SynthesisContext_& context);

			void
			setAudioInput(Generator gen);
			void
			setAmplitudeInput(Generator gen);

			void
			setMakeupGain(ControlGenerator gen) {
				makeupGainGen_ = gen;
			}
			void
			setAttack(ControlGenerator gen) {
				attackGen_ = gen;
			}
			void
			setRelease(ControlGenerator gen) {
				releaseGen_ = gen;
			}
			void
			setThreshold(ControlGenerator gen) {
				threshGen_ = gen;
			}
			void
			setRatio(ControlGenerator gen) {
				ratioGen_ = gen;
			}
			void
			setLookahead(ControlGenerator gen) {
				lookaheadGen_ = gen;
			}

			//Set whether is a limiter - limiters will hard clip to threshold in worst case.
			void
			setIsLimiter(bool isLimiter) {
				isLimiter_ = isLimiter;
			}

			//Externally set whether operates on one or two channels.
			void
			setIsStereo(bool isStereo);
		};

		inline void
		Compressor_::tick(NAudioFrames& frames, const SynthesisContext_& context) {
			if(context.forceNewOutput || lastFrameIndex_ != context.elapsedFrames) {
				//Get amp input frames.
				amplitudeInput_.tick(ampInputFrames_, context);
			}

			Effect_::tick(frames, context);
		}

		inline void
		Compressor_::tickThrough(NAudioFrames& inFrames, NAudioFrames& outFrames, const SynthesisContext_& context) {
			ampInputFrames_.Copy(inFrames);
			Effect_::tickThrough(inFrames, outFrames, context);
		}

		inline void
		Compressor_::computeSynthesisBlock(const SynthesisContext_& context) {
			//Tick all scalar parameters.
			float attackCoef = t60ToOnePoleCoef(Max(0.0f, attackGen_.tick(context).value));
			float releaseCoef = t60ToOnePoleCoef(Max(0.0f, releaseGen_.tick(context).value));
			float threshold = Max(0.0f, threshGen_.tick(context).value);
			float ratio = Max(0.0f, ratioGen_.tick(context).value);
			float lookaheadTime = Max(0.0f, lookaheadGen_.tick(context).value);

			//Absolute value of amplitude frames in prep for amp envelope.
			float* ampData = &ampInputFrames_[0];

			for(unsigned int i = 0; i < ampInputFrames_.Size(); ++i, ++ampData) {
				*ampData = fabsf(*ampData);
			}

			//Iterate through samples.
			unsigned int nChannels = outputFrames_.Channels();

			float ampInputValue;
			float gainValue;
			float gainTarget;

			float* outptr = &outputFrames_[0];
			float* dryptr = &dryFrames_[0];

			ampData = &ampInputFrames_[0];

			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				//Tick input into lookahead delay and get amplitude input value - max of left/right.
				ampInputValue = 0;

				for(unsigned int j = 0; j < nChannels; ++j) {
					lookaheadDelayLine_.tickIn(*dryptr++, j);
					ampInputValue = Max(ampInputValue, *ampData++);
				}

				//Smooth amplitude input.
				if(ampInputValue >= ampEnvValue_) {
					onePoleLPFTick(ampInputValue, ampEnvValue_, attackCoef);
				}
				else {
					onePoleLPFTick(ampInputValue, ampEnvValue_, releaseCoef);
				}

				//Calculate gain value.
				if(ampEnvValue_ <= threshold) {
					gainValue = 1.0f;
				}
				else {
					//Compensate for ratio.
					gainTarget = threshold + ((ampEnvValue_ - threshold) / ratio);
					gainValue = gainTarget / ampEnvValue_;
				}

				//Smooth gain value.
				if(gainValue <= gainEnvValue_) {
					onePoleLPFTick(gainValue, gainEnvValue_, attackCoef);
				}
				else {
					onePoleLPFTick(gainValue, gainEnvValue_, releaseCoef);
				}

				//Apply gain.
				for(unsigned int j = 0; j < nChannels; ++j) {
					*outptr++ = lookaheadDelayLine_.tickOut(lookaheadTime, j) * gainEnvValue_;
				}

				lookaheadDelayLine_.advance();
			}

			float makeupGain = Max(0.0f, makeupGainGen_.tick(context).value);
			outptr = &outputFrames_[0];

			for(unsigned int i = 0; i < outputFrames_.Size(); ++i) {
				*outptr++ *= makeupGain;
			}

			if(isLimiter_) {
				//Clip to threshold in worst case (minor distortion introduced but much preferable to wrapping distortion).
				outptr = &outputFrames_[0];

				for(unsigned int i = 0; i < outputFrames_.Size(); ++i, ++outptr) {
					*outptr = Clamp(*outptr, -threshold, threshold);
				}
			}
		}
	}

	class Compressor : public TemplatedEffect<Compressor, NAudio_DSP::Compressor_> {
	public:
		Compressor(float threshold = 0.5f, float ratio = 2.0f, float attack = 0.001f, float release = 0.05f, float lookahead = 0.001f);

		//Input for audio to be compressed.
		Compressor&
		audioInput(Generator input) {
			this->gen()->setInput(input);
			return(*this);
		}

		//Input for audio for compression amplitude envelope.
		Compressor& sidechainInput(Generator input) {
			this->gen()->setAmplitudeInput(input);
			return(*this);
		}

		void
		setIsStereo(bool isStereo) {
			this->gen()->setIsStereo(isStereo);
		}

		NAUDIO_MAKE_CTRL_GEN_SETTERS(Compressor, attack, setAttack);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Compressor, release, setRelease);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Compressor, threshold, setThreshold);		//Linear - Use dBToLin to convert from dB.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Compressor, ratio, setRatio);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Compressor, lookahead, setLookahead);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Compressor, makeupGain, setMakeupGain);

		//TODO: option for RMS
	};

	//Special case of compressor where ratio is fixed to infinite and attack time is fixed to 0.1 ms. Used for peak limiting.
	class Limiter : public TemplatedEffect<Limiter, NAudio_DSP::Compressor_> {
	public:
		Limiter();

		void
		setIsStereo(bool isStereo) {
			this->gen()->setIsStereo(isStereo);
		}

		NAUDIO_MAKE_CTRL_GEN_SETTERS(Limiter, release, setRelease);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Limiter, threshold, setThreshold);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Limiter, lookahead, setLookahead);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Limiter, makeupGain, setMakeupGain);
	};
}