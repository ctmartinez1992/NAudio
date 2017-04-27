#pragma once

#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlDelay_ : public ControlConditioner_ {
		private:
			long readHead_;
			long writeHead_;
			long maxDelay_;		//Synthesis blocks of delay.

			std::vector<ControlGeneratorOutput> delayLine_;

			ControlGenerator delayTimeCtrlGen_;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlDelay_();

			void
			initialize(float maxDelayTime);

			void
			setDelayTimeGen(ControlGenerator gen) {
				delayTimeCtrlGen_ = gen;
			}
		};

		inline void
		ControlDelay_::computeOutput(const SynthesisContext_& context) {
			delayLine_[writeHead_] = input_.tick(context);

			ControlGeneratorOutput delayTimeOutput = delayTimeCtrlGen_.tick(context);

			if(delayTimeOutput.triggered) {
				unsigned int delayBlocks = (unsigned int)(Max(delayTimeOutput.value * SampleRate() / kSynthesisBlockSize, 1.0f));

				if((long)delayBlocks >= maxDelay_) {
					LOG(NLOG_DEBUG, "Delay time greater than maximum delay (defaults to 1 scond). Use constructor to set max delay. Example: ControlDelay(2.0);");
				}

				readHead_ = writeHead_ - delayBlocks;

				if(readHead_ < 0l) {
					readHead_ += maxDelay_;
				}
			}

			output_ = delayLine_[readHead_];

			if(++writeHead_ >= maxDelay_) {
				writeHead_ = 0l;
			}

			if(++readHead_ >= maxDelay_) {
				readHead_ = 0l;
			}
		}
	}

	class ControlDelay : public TemplatedControlConditioner<ControlDelay, NAudio_DSP::ControlDelay_> {
	public:
		ControlDelay(float maxDelayTime = 1.0f);

		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlDelay, delayTime, setDelayTimeGen);
	};
}