#include "Reverb.h"

#define NAUDIO_REVERB_MIN_TAPS			2
#define NAUDIO_REVERB_MAX_TAPS			20

#define NAUDIO_REVERB_MIN_WALL_DIST		3.0f		//meters.
#define NAUDIO_REVERB_MAX_WALL_DIST		10.0f		//meters.
#define NAUDIO_REVERB_SOS				340.0f		//m/s, approximate.
#define NAUDIO_REVERB_AIRDECAY			-0.159f		//db/m, approximate.

#define NAUDIO_REVERB_FUDGE_AMT			0.05f		//Amount of randomization introduced to reflection times.

//Number of FF comb filters per channel.
#define NAUDIO_REVERB_N_COMBS			8
#define NAUDIO_REVERB_MIN_COMB_TIME		0.015f
#define NAUDIO_REVERB_MAX_COMB_TIME		0.035f
#define NAUDIO_REVERB_STEREO_SPREAD		0.001f

//Number of allpass filters per channel.
#define NAUDIO_REVERB_N_ALLPASS			4
#define NAUDIO_REVERB_ALLPASS_COEF		0.5f

namespace NAudio {
	namespace NAudio_DSP {
		ImpulseDiffuserAllpass::ImpulseDiffuserAllpass(float delay, float coef) {
			delay_ = delay;
			coef_ = coef;

			delayBack_.initialize(delay, 1u);
			delayForward_.initialize(delay, 1u);
			delayBack_.setInterpolates(false);
			delayForward_.setInterpolates(false);
		}

		ImpulseDiffuserAllpass::ImpulseDiffuserAllpass(const ImpulseDiffuserAllpass& other) {
			delay_ = other.delay_;
			coef_ = other.coef_;

			delayBack_.initialize(delay_, 1u);
			delayForward_.initialize(delay_, 1u);
			delayBack_.setInterpolates(false);
			delayForward_.setInterpolates(false);
		}

		//Changing these will change the character of the late-stage reverb.
		static const float combTimeScales_[NAUDIO_REVERB_N_COMBS] = { 1.17f, 1.12f, 1.02f, 0.97f, 0.95f, 0.88f, 0.84f, 0.82f };
		static const float allpassTimes_[NAUDIO_REVERB_N_ALLPASS] = { 0.0051f, 0.010f, 0.012f, 0.00833f };

		Reverb_::Reverb_() {
			setIsStereoOutput(true);

			//Default to 50% wet.
			setDryLevelGen(FixedValue(0.5f));
			setWetLevelGen(FixedValue(0.5f));

			workspaceFrames_[0].Resize(kSynthesisBlockSize, 1u, 0.0f);
			workspaceFrames_[1].Resize(kSynthesisBlockSize, 1u, 0.0f);
			preOutputFrames_[0].Resize(kSynthesisBlockSize, 1u, 0.0f);
			preOutputFrames_[1].Resize(kSynthesisBlockSize, 1u, 0.0f);

			preDelayLine_.initialize(0.1f, 1u);
			reflectDelayLine_.initialize(0.1f, 1u);

			inputLPF_.setIsStereoInput(false);
			inputLPF_.setIsStereoInput(false);

			inputLPF_.Q(0.707f);
			inputHPF_.Q(0.707f);

			preDelayTimeCtrlGen_ = ControlValue(0.01f);
			inputFiltBypasCtrlGen_ = ControlValue(false);
			densityCtrlGen_ = ControlValue(0.5f);
			roomShapeCtrlGen_ = ControlValue(0.5f);
			roomSizeCtrlGen_ = ControlValue(0.5f);
			decayTimeCtrlGen_ = ControlValue(1.0f);
			stereoWidthCtrlGen_ = ControlValue(0.5f);

			setInputLPFCutoffCtrlGen(ControlValue(10000.0f));
			setInputHPFCutoffCtrlGen(ControlValue(20.0f));

			for(unsigned int i = 0; i < NAUDIO_REVERB_N_COMBS; ++i) {
				ControlValue delayL = ControlValue(0.01f);
				ControlValue delayR = ControlValue(0.01f);
				ControlValue scaleL = ControlValue(0.5f);
				ControlValue scaleR = ControlValue(0.5f);

				combFilterDelayTimes_[NAUDIO_LEFT].push_back(delayL);
				combFilterDelayTimes_[NAUDIO_RIGHT].push_back(delayR);
				combFilterScaleFactors_[NAUDIO_LEFT].push_back(scaleL);
				combFilterScaleFactors_[NAUDIO_RIGHT].push_back(scaleR);

				combFilters_[NAUDIO_LEFT].push_back(FilteredFBCombFilter6(0.01f, 0.125f).delayTime(delayL).scaleFactor(scaleL));
				combFilters_[NAUDIO_RIGHT].push_back(FilteredFBCombFilter6(0.01f, 0.125f).delayTime(delayR).scaleFactor(scaleR));
			}

			for(unsigned int i = 0; i < NAUDIO_REVERB_N_ALLPASS; ++i) {
				allpassFilters_[NAUDIO_LEFT].push_back(ImpulseDiffuserAllpass(allpassTimes_[i], NAUDIO_REVERB_ALLPASS_COEF));
				allpassFilters_[NAUDIO_RIGHT].push_back(ImpulseDiffuserAllpass(allpassTimes_[i] + NAUDIO_REVERB_STEREO_SPREAD, NAUDIO_REVERB_ALLPASS_COEF));
			}

			setDecayLPFCtrlGen(ControlValue(12000.0f));
			setDecayHPFCtrlGen(ControlValue(60.0f));
		}

		void
		Reverb_::updateDelayTimes(const SynthesisContext_& context) {
			ControlGeneratorOutput densityOutput = densityCtrlGen_.tick(context);
			ControlGeneratorOutput shapeOutput = roomShapeCtrlGen_.tick(context);
			ControlGeneratorOutput sizeOutput = roomSizeCtrlGen_.tick(context);
			ControlGeneratorOutput decayOutput = decayTimeCtrlGen_.tick(context);

			if(densityOutput.triggered || shapeOutput.triggered || sizeOutput.triggered) {
				reflectTapTimes_.clear();
				reflectTapScale_.clear();

				//Compute base round-trip times from listener to wall, based on shape and size values.
				float shape = Clamp(shapeOutput.value, 0.0f, 1.0f);
				float size = Clamp(sizeOutput.value, 0.0f, 1.0f);

				float wDist1 = MapFloat(size, 0.0f, 1.0f, NAUDIO_REVERB_MIN_WALL_DIST, NAUDIO_REVERB_MAX_WALL_DIST, true);
				float wDist2 = MapFloat(size * (1.1f - shape), 0.0f, 1.0f, NAUDIO_REVERB_MIN_WALL_DIST, NAUDIO_REVERB_MAX_WALL_DIST, true);

				unsigned int nTaps = (unsigned int)MapFloat(densityOutput.value, 0.0f, 1.0f, NAUDIO_REVERB_MIN_TAPS, NAUDIO_REVERB_MAX_TAPS, true);

				float tapScale = 1.0f / Max(2.0f, sqrtf(nTaps));

				for(unsigned int i = 0; i < nTaps; ++i) {
					float dist = (i % 2 == 0 ? wDist1 : wDist2) * (1.0f + NRNG::GetInstance()->GetFloat(-NAUDIO_REVERB_FUDGE_AMT, NAUDIO_REVERB_FUDGE_AMT));

					reflectTapTimes_.push_back(dist / NAUDIO_REVERB_SOS);
					reflectTapScale_.push_back(DBToLin(dist * NAUDIO_REVERB_AIRDECAY)*tapScale);
				}
			}

			//If decay or room size have changed, need to update comb times/scales.
			if(sizeOutput.triggered || decayOutput.triggered) {
				float decayTime = decayOutput.value;
				float baseCombDelayTime = MapFloat(sizeOutput.value, 0.0f, 1.0f, NAUDIO_REVERB_MIN_COMB_TIME, NAUDIO_REVERB_MAX_COMB_TIME, true);

				for(unsigned int i = 0; i < NAUDIO_REVERB_N_COMBS; ++i) {
					float scaledDelayTime = combTimeScales_[i % NAUDIO_REVERB_N_COMBS] * baseCombDelayTime;

					combFilterDelayTimes_[NAUDIO_LEFT][i].value(scaledDelayTime);
					combFilterDelayTimes_[NAUDIO_RIGHT][i].value(scaledDelayTime + NAUDIO_REVERB_STEREO_SPREAD);
					combFilterScaleFactors_[NAUDIO_LEFT][i].value(powf(10.0f, (-3.0f * scaledDelayTime / decayTime)));
					combFilterScaleFactors_[NAUDIO_RIGHT][i].value(powf(10.0f, (-3.0f * (scaledDelayTime + NAUDIO_REVERB_STEREO_SPREAD) / decayTime)));
				}
			}
		}

		void
		Reverb_::setDecayLPFCtrlGen(ControlGenerator gen) {
			for(unsigned int i = 0; i < NAUDIO_REVERB_N_COMBS; ++i) {
				combFilters_[NAUDIO_LEFT][i].lowpassCutoff(gen);
				combFilters_[NAUDIO_RIGHT][i].lowpassCutoff(gen);
			}
		}

		void
		Reverb_::setDecayHPFCtrlGen(ControlGenerator gen) {
			for(unsigned int i = 0; i < NAUDIO_REVERB_N_COMBS; ++i) {
				combFilters_[NAUDIO_LEFT][i].highpassCutoff(gen);
				combFilters_[NAUDIO_RIGHT][i].highpassCutoff(gen);
			}
		}
	}
}