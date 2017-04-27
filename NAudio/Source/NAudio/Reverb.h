#pragma once

#include "Effect.h"
#include "DelayUtils.h"
#include "CombFilter.h"
#include "Filters.h"
#include "MonoToStereoPanner.h"
#include "NRNG/NRNG.h"

namespace NAudio {
	namespace NAudio_DSP {
		//Allpass filter for use with reverb. This is not a Generator_ subclass and is optimized for the purposes of the Reverb_ class.
		class ImpulseDiffuserAllpass {
		protected:
			DelayLine delayForward_;
			DelayLine delayBack_;

			float delay_;
			float coef_;

		public:
			ImpulseDiffuserAllpass(float delay, float coef);
			ImpulseDiffuserAllpass(const ImpulseDiffuserAllpass& other);

			void
			tickThrough(NAudioFrames& frames);
		};

		inline void
		ImpulseDiffuserAllpass::tickThrough(NAudio::NAudioFrames& frames) {
			float* dptr = &frames[0];
			float y;

			for(int i = 0; i < kSynthesisBlockSize; ++i) {
				//Feedback stage.
				y = *dptr + delayBack_.tickOut(delay_) * coef_;
				delayBack_.tickIn(y);
				delayBack_.advance();

				//Feed forward stage.
				*dptr++ = (1.0f + coef_)*delayForward_.tickOut(delay_) - y;
				delayForward_.tickIn(y);
				delayForward_.advance();
			}
		}

		//Moorer-Schroeder style Artificial Reverb effect: Pre-delay; Input filter; Early reflection taps; Decay time and decay filtering; Variable "Room size"; Variable stereo width.
		//TODO: Reverb fb comb cutoff setting parameters should be normalized value, not Hz. Not a "true" cutoff.
		//TODO: Highpass portion of fb comb filter is too wide. Should be LPF24.
		//TODO: More deterministic early reflection time scattering.
		class Reverb_ : public WetDryEffect_ {
		protected:
			//Filters and delay lines.
			DelayLine preDelayLine_;
			DelayLine reflectDelayLine_;

			LPF12 inputLPF_;
			HPF12 inputHPF_;

			std::vector<float> reflectTapTimes_;
			std::vector<float> reflectTapScale_;

			//Comb filters.
			std::vector<FilteredFBCombFilter6> combFilters_[2];
			std::vector<ControlValue> combFilterDelayTimes_[2];
			std::vector<ControlValue> combFilterScaleFactors_[2];

			//Allpass filters.
			std::vector<ImpulseDiffuserAllpass> allpassFilters_[2];

			//Signal vector workspaces.
			NAudioFrames workspaceFrames_[2];
			NAudioFrames preOutputFrames_[2];

			//Input generators.
			ControlGenerator preDelayTimeCtrlGen_;
			ControlGenerator inputFiltBypasCtrlGen_;
			ControlGenerator roomSizeCtrlGen_;
			ControlGenerator roomShapeCtrlGen_;
			ControlGenerator densityCtrlGen_;				//Affects number of early reflection taps.

			ControlGenerator decayTimeCtrlGen_;
			ControlGenerator stereoWidthCtrlGen_;

			void
			updateDelayTimes(const SynthesisContext_& context);

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			Reverb_();

			void
			setPreDelayTimeCtrlGen(ControlGenerator gen) {
				preDelayTimeCtrlGen_ = gen;
			}

			void
			setInputFiltBypassCtrlGen(ControlGenerator gen) {
				inputFiltBypasCtrlGen_ = gen;
			}

			void
			setInputLPFCutoffCtrlGen(ControlGenerator gen) {
				inputLPF_.cutoff(gen);
			}

			void
			setInputHPFCutoffCtrlGen(ControlGenerator gen) {
				inputHPF_.cutoff(gen);
			}

			void
			setRoomSizeCtrlGen(ControlGenerator gen) {
				roomSizeCtrlGen_ = gen;
			}

			void
			setRoomShapeCtrlGen(ControlGenerator gen) {
				roomShapeCtrlGen_ = gen;
			}

			void
			setDensityCtrlGen(ControlGenerator gen) {
				densityCtrlGen_ = gen;
			}

			void
			setDecayTimeCtrlGen(ControlGenerator gen) {
				decayTimeCtrlGen_ = gen;
			}

			void
			setStereoWidthCtrlGen(ControlGenerator gen) {
				stereoWidthCtrlGen_ = gen;
			}

			//These are special setters, they will be passed to all the comb filters.
			void
			setDecayLPFCtrlGen(ControlGenerator gen);

			void
			setDecayHPFCtrlGen(ControlGenerator gen);
		};

		inline void
		Reverb_::computeSynthesisBlock(const SynthesisContext_& context) {
			updateDelayTimes(context);

			//Pass thru input filters.
			if(inputFiltBypasCtrlGen_.tick(context).value == 0.0f) {
				inputLPF_.tickThrough(dryFrames_, workspaceFrames_[0], context);
				inputHPF_.tickThrough(workspaceFrames_[0], workspaceFrames_[0], context);
			}
			else {
				workspaceFrames_[0].Copy(dryFrames_);
			}

			float* wkptr0 = &(workspaceFrames_[0])[0];
			float* wkptr1 = &(workspaceFrames_[1])[0];

			//Pass thru pre-delay, input filters, and sum the early reflections.
			float preDelayTime = preDelayTimeCtrlGen_.tick(context).value;

			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				//Filtered input is in w0. Predelay output is in w1.
				//Pre-delay.
				preDelayLine_.tickIn(*wkptr0);
				*wkptr1 = preDelayLine_.tickOut(preDelayTime);
				preDelayLine_.advance();

				//Taps, write back to w0.
				reflectDelayLine_.tickIn(*wkptr1++);

				*wkptr0 = 0;

				for(unsigned int t = 0; t < reflectTapTimes_.size(); ++t) {
					*wkptr0 += reflectDelayLine_.tickOut(reflectTapTimes_[t]) * reflectTapScale_[t];
				}

				reflectDelayLine_.advance();
				wkptr0++;
			}

			//Comb filers.
			preOutputFrames_[NAUDIO_LEFT].Clear();
			preOutputFrames_[NAUDIO_RIGHT].Clear();

			for(unsigned int i = 0; i < combFilters_[NAUDIO_LEFT].size(); ++i) {
				combFilters_[NAUDIO_LEFT][i].tickThrough(workspaceFrames_[0], workspaceFrames_[1], context);
				preOutputFrames_[NAUDIO_LEFT] += workspaceFrames_[1];
				combFilters_[NAUDIO_RIGHT][i].tickThrough(workspaceFrames_[0], workspaceFrames_[1], context);
				preOutputFrames_[NAUDIO_RIGHT] += workspaceFrames_[1];
			}

			//Allpass filters.
			for(unsigned int i = 0; i < allpassFilters_[NAUDIO_LEFT].size(); ++i) {
				allpassFilters_[NAUDIO_LEFT][i].tickThrough(preOutputFrames_[NAUDIO_LEFT]);
				allpassFilters_[NAUDIO_RIGHT][i].tickThrough(preOutputFrames_[NAUDIO_RIGHT]);
			}

			//Interleave pre-output frames into output frames.
			float* outptr = &outputFrames_[0];
			float* preoutptrL = &preOutputFrames_[NAUDIO_LEFT][0];
			float* preoutptrR = &preOutputFrames_[NAUDIO_RIGHT][0];

			float spreadValue = Clamp(1.0f - stereoWidthCtrlGen_.tick(context).value, 0.0f, 1.0f);
			float normValue = (1.0f / (1.0f + spreadValue)) * 0.04f;										//Scale back levels quite a bit.

			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				*outptr++ = (*preoutptrL + (spreadValue * (*preoutptrR)))*normValue;
				*outptr++ = (*preoutptrR++ + (spreadValue * (*preoutptrL++)))*normValue;
			}
		}
	}

	class Reverb : public TemplatedWetDryEffect<Reverb, NAudio_DSP::Reverb_> {
	public:
		//Initial delay before passing through reverb.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, preDelayTime, setPreDelayTimeCtrlGen);

		//Non-zero value will disable input filtering.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, bypassInputFilter, setInputFiltBypassCtrlGen);

		//Value in Hz of cutoff for input LPF.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, inputLPFCutoff, setInputLPFCutoffCtrlGen);
		
		//Value in Hz of cutoff for input HPF.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, inputHPFCutoff, setInputHPFCutoffCtrlGen);

		//Value 0-1, affects number of early reflections.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, density, setDensityCtrlGen);

		//Value 0-1, affects distribution of early reflections. 0 is perfectly square room, 1 is long, narrow room.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, roomShape, setRoomShapeCtrlGen);

		//Value 0-1, affects spacing of early reflections.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, roomSize, setRoomSizeCtrlGen);

		//Value in seconds of overall decay time.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, decayTime, setDecayTimeCtrlGen);

		//Value in Hz of cutoff of decay LPF.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, decayLPFCutoff, setDecayLPFCtrlGen);

		//Value in Hz of cutoff of decay HPF.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, decayHPFCutoff, setDecayHPFCtrlGen);

		//Value 0-1 for stereo width.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(Reverb, stereoWidth, setStereoWidthCtrlGen);
	};
}