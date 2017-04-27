#pragma once

#include "Generator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class Effect_ : public Generator_ {
		protected:
			Generator input_;
			NAudioFrames dryFrames_;
			ControlGenerator bypassGen_;

			bool isStereoInput_;

		public:
			Effect_();

			void
			setBypassCtrlGen(ControlGenerator gen) {
				bypassGen_ = gen;
			}

			virtual void
			setInput(Generator input) {
				input_ = input;
			}

			virtual void
			setIsStereoInput(bool stereo);

			bool
			isStereoInput() {
				return(isStereoInput_);
			}

			virtual void
			tick(NAudioFrames& frames, const SynthesisContext_& context);

			//Apply effect directly to passed in frames (output in-place). Do NOT mix calls to tick() with calls to tickThrough().
			virtual void
			tickThrough(NAudioFrames& inFrames, NAudioFrames& outFrames, const SynthesisContext_& context);
		};

		inline void
		Effect_::setIsStereoInput(bool stereo) {
			if(stereo != isStereoInput_) {
				dryFrames_.Resize(kSynthesisBlockSize, stereo ? 2u : 1u, 0.0f);
			}

			isStereoInput_ = stereo;
		}

		//Overridden tick - pre-ticks input to fill dryFrames_. Subclasses don't need to tick input, dryFrames_ contains "dry" input by the time. computeSynthesisBlock() is called.
		inline void
		Effect_::tick(NAudioFrames& frames, const SynthesisContext_& context) {
			//Check context to see if we need new frames.
			if(context.elapsedFrames == 0 || lastFrameIndex_ != context.elapsedFrames) {
				//Get dry input frames.
				input_.tick(dryFrames_, context);
				computeSynthesisBlock(context);

				//Bypass processing - still need to compute block so all generators stay in sync.
				if(bypassGen_.tick(context).value != 0.0f) {
					outputFrames_.Copy(dryFrames_);
				}

				lastFrameIndex_ = context.elapsedFrames;
			}

			frames.Copy(outputFrames_);

			if(!isfinite(frames(0, 0u))) {
				LOG(NLOG_ERROR, "NaN or inf detected.");
			}
		}

		inline void
		Effect_::tickThrough(NAudioFrames& inFrames, NAudioFrames& outFrames, const SynthesisContext_& context) {
			//Do not check context here, assume each call should produce new output.
			dryFrames_.Copy(inFrames);
			computeSynthesisBlock(context);

			//Bypass processing - still need to compute block so all generators stay in sync.
			if(bypassGen_.tick(context).value != 0.0f) {
				outFrames.Copy(dryFrames_);
			}
			else {
				outFrames.Copy(outputFrames_);
			}
		}
	}

	template<class EffectType, class EffectType_>
	class TemplatedEffect : public TemplatedGenerator<EffectType_> {
	public:
		//This cast is not safe - up to implementation to ensure that templated EffectType_ is actually an Effect_ subclass.
		virtual EffectType&
		input(Generator input) {
			this->gen()->setInput(input);
			return(static_cast<EffectType&>(*this));
		}

		void
		tickThrough(NAudioFrames& inFrames, const NAudio_DSP::SynthesisContext_& context) {
			this->gen()->tickThrough(inFrames, inFrames, context);
		}

		void
		tickThrough(NAudioFrames& inFrames, NAudioFrames& outFrames, const NAudio_DSP::SynthesisContext_& context) {
			this->gen()->tickThrough(inFrames, outFrames, context);
		}

		void
		setIsStereoInput(bool isStereoInput) {
			this->gen()->setIsStereoInput(isStereoInput);
		}

		NAUDIO_MAKE_CTRL_GEN_SETTERS(EffectType, bypass, setBypassCtrlGen);
	};

	//Signal flow operator - sets lhs as input to rhs.
	template<class EffectType, class EffectType_>
	static EffectType
	operator>>(Generator lhs, TemplatedEffect<EffectType, EffectType_> rhs) {
		return(rhs.input(lhs));
	}

	//Wet/Dry mix effects.
	namespace NAudio_DSP {
		//WetDryEffect_ extends the basic functionality of Effect_ but also adds a wet/dry mix to the basic tick() cycle.
		class WetDryEffect_ : public Effect_ {
		protected:
			Generator dryLevelGen_;
			Generator wetLevelGen_;
			NAudioFrames mixWorkspace_;

		public:
			WetDryEffect_();

			void
			setDryLevelGen(Generator gen) {
				dryLevelGen_ = gen;
			}

			void
			setWetLevelGen(Generator gen) {
				wetLevelGen_ = gen;
			}

			virtual void tick(NAudioFrames& frames, const SynthesisContext_& context);

			//Apply effect directly to passed in frames (output in-place). Do NOT mix calls to tick() with calls to tickThrough().
			virtual void tickThrough(NAudioFrames& inFrames, NAudioFrames& outFrames, const SynthesisContext_& context);
		};
		
		//Overridden tick, pre-ticks input to fill dryFrames_. Subclasses don't need to tick input, dryFrames_ contains "dry" input by the time. computeSynthesisBlock() is called.
		inline void
		WetDryEffect_::tick(NAudioFrames& frames, const SynthesisContext_& context) {
			//Check context to see if we need new frames.
			if(context.elapsedFrames == 0 || lastFrameIndex_ != context.elapsedFrames) {
				//Get dry input frames.
				input_.tick(dryFrames_, context);
				computeSynthesisBlock(context);

				//Bypass processing - still need to compute block so all generators stay in sync.
				if(bypassGen_.tick(context).value != 0.0f) {
					outputFrames_.Copy(dryFrames_);
				}
				else {
					//Do not apply dry/wet levels if wet flag is set, offers minor CPU usage optimization.
					wetLevelGen_.tick(mixWorkspace_, context);
					outputFrames_ *= mixWorkspace_;
					dryLevelGen_.tick(mixWorkspace_, context);
					dryFrames_ *= mixWorkspace_;

					outputFrames_ += dryFrames_;
				}

				lastFrameIndex_ = context.elapsedFrames;
			}

			//Copy synthesis block to frames passed in.
			frames.Copy(outputFrames_);

			if(!isfinite(frames(0, 0u))) {
				LOG(NLOG_ERROR, "NaN or inf detected.");
			}
		}

		inline void
		WetDryEffect_::tickThrough(NAudioFrames& inFrames, NAudioFrames& outFrames, const SynthesisContext_& context) {
			//Do not check context here, assume each call should produce new output.
			dryFrames_.Copy(inFrames);
			computeSynthesisBlock(context);

			//Bypass processing - still need to compute block so all generators stay in sync.
			if(bypassGen_.tick(context).value != 0.0f) {
				outFrames.Copy(dryFrames_);
			}
			else {
				wetLevelGen_.tick(mixWorkspace_, context);
				outputFrames_ *= mixWorkspace_;
				dryLevelGen_.tick(mixWorkspace_, context);
				dryFrames_ *= mixWorkspace_;

				outputFrames_ += dryFrames_;
				outFrames.Copy(outputFrames_);
			}
		}
	}

	template<class EffectType, class EffectType_>
	class TemplatedWetDryEffect : public TemplatedGenerator<EffectType_> {
	public:
		// This cast is not safe, up to implementation to ensure that templated EffectType_ is actually an Effect_ subclass.
		virtual EffectType&
		input(Generator input) {
			this->gen()->setInput(input);
			return(static_cast<EffectType&>(*this));
		}

		void
		tickThrough(NAudioFrames& inFrames, const NAudio_DSP::SynthesisContext_& context) {
			this->gen()->tickThrough(inFrames, inFrames, context);
		}

		void
		tickThrough(NAudioFrames& inFrames, NAudioFrames& outFrames, const NAudio_DSP::SynthesisContext_& context) {
			this->gen()->tickThrough(inFrames, outFrames, context);
		}

		void
		setIsStereoInput(bool isStereoInput) {
			this->gen()->setIsStereoInput(isStereoInput);
		}

		NAUDIO_MAKE_CTRL_GEN_SETTERS(EffectType, bypass, setBypassCtrlGen);

		//Defaults to 1.0.
		NAUDIO_MAKE_GEN_SETTERS(EffectType, wetLevel, setWetLevelGen);

		//Defaults to 0.0 (full wet).
		NAUDIO_MAKE_GEN_SETTERS(EffectType, dryLevel, setDryLevelGen);
	};

	//Signal flow operator, sets lhs as input to rhs.
	template<class EffectType, class EffectType_>
	static EffectType
	operator>>(Generator lhs, TemplatedWetDryEffect<EffectType, EffectType_> rhs) {
		return(rhs.input(lhs));
	}
}