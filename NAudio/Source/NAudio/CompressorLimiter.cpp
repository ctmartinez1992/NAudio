#include "CompressorLimiter.h"

namespace NAudio {
	namespace NAudio_DSP {
		Compressor_::Compressor_() :
			ampEnvValue_(0.0f), gainEnvValue_(1.0f),
			isLimiter_(false)
		{
			ampInputFrames_.Resize(kSynthesisBlockSize, 1u, 0.0f);

			lookaheadDelayLine_.initialize(0.01f, 2u);
			lookaheadDelayLine_.setInterpolates(false);		//No real need to interpolate here for lookahead.

			makeupGainGen_ = ControlValue(1.0f);
		}

		//Default inherited input method sets both audio signal and amplitude signal as input so incoming signal is compressed based on its own amplitude.
		void
		Compressor_::setInput(Generator input) {
			setAmplitudeInput(input);
			setAudioInput(input);
		}

		void
		Compressor_::setAudioInput(Generator gen) {
			input_ = gen;

			setIsStereoInput(gen.isStereoOutput());
			setIsStereoOutput(gen.isStereoOutput());
		}

		void
		Compressor_::setAmplitudeInput(Generator gen) {
			amplitudeInput_ = gen;
			ampInputFrames_.Resize(kSynthesisBlockSize, amplitudeInput_.isStereoOutput() ? 2u : 1u, 0.0f);
		}

		void
		Compressor_::setIsStereo(bool isStereo) {
			setIsStereoInput(isStereo);
			setIsStereoOutput(isStereo);

			ampInputFrames_.Resize(kSynthesisBlockSize, isStereo ? 2u : 1u, 0.0f);
		}
	}

	Compressor::Compressor(float threshold, float ratio, float attack, float release, float lookahead) {
		gen()->setThreshold(ControlValue(threshold));
		gen()->setRatio(ControlValue(ratio));
		gen()->setAttack(ControlValue(attack));
		gen()->setRelease(ControlValue(release));
		gen()->setLookahead(ControlValue(lookahead));
	}

	Limiter::Limiter() {
		gen()->setIsLimiter(true);
		gen()->setAttack(ControlValue(0.0001f));
		gen()->setLookahead(ControlValue(0.003f));
		gen()->setRelease(ControlValue(0.080f));
		gen()->setThreshold(ControlValue(DBToLin(-0.1f)));
		gen()->setRatio(ControlValue(std::numeric_limits<float>::infinity()));
	}
}