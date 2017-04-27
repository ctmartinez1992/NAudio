#pragma once

#include "Generator.h"
#include "ControlValue.h"

namespace NAudio {
	namespace NAudio_DSP {
		class RampedValue_ : public Generator_ {
		protected:
			ControlGenerator targetGen_;
			ControlGenerator lengthGen_;
			ControlGenerator valueGen_;

			float target_;
			float last_;
			float inc_;

			unsigned long count_;
			unsigned long len_;

			bool finished_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			RampedValue_();
			~RampedValue_();

			void
			setValueGen(ControlGenerator value);

			void
			setTargetGen(ControlGenerator target);

			void
			setLengthGen(ControlGenerator length);

			//Internal state setters.
			void
			updateValue(float value);

			void
			updateTarget(float target, unsigned long lengthSamp);

			bool
			isFinished();
		};

		inline void
		RampedValue_::computeSynthesisBlock(const SynthesisContext_& context) {
			//First set the value, if necessary (abort ramp, go immediately to value).
			ControlGeneratorOutput valueOutput = valueGen_.tick(context);

			if(valueOutput.triggered) {
				updateValue(valueOutput.value);
			}

			//Then update the target or ramp length (start a new ramp).
			ControlGeneratorOutput lengthOutput = lengthGen_.tick(context);
			ControlGeneratorOutput targetOutput = targetGen_.tick(context);

			if(lengthOutput.triggered || targetOutput.triggered) {
				unsigned long lSamp = (unsigned long)(lengthOutput.value * NAudio::SampleRate());
				updateTarget(targetOutput.value, lSamp);
			}

			float* fdata = &outputFrames_[0];

			unsigned int nFrames = kSynthesisBlockSize;
			unsigned int stride = outputFrames_.Channels();

			//Edge case.
			if(count_ == len_) {
				finished_ = true;
			}

			if(finished_) {
				for(unsigned int i = 0; i < nFrames; ++i) {
					*fdata = last_;
					fdata += stride;
				}
			}
			else {
				//Figure out if we will finish the ramp in this tick.
				unsigned long remainder = count_ > len_ ? 0ul : len_ - count_;

				if(remainder < nFrames) {
					//Fill part of the ramp. No need to check if remainder is 1 when using vramp uses algorithm out[n] = start + n * increment;.
					for(unsigned int i = 0; i < remainder; ++i) {
						last_ += inc_;
						*fdata = last_;
						fdata += stride;
					}

					for(unsigned int i = remainder; i < nFrames; ++i) {
						*fdata = target_;
						fdata += stride;
					}

					count_ = len_;
					last_ = target_;

					finished_ = true;
				}
				else {
					//Fill the whole ramp.
					for(unsigned int i = 0; i < nFrames; ++i) {
						last_ += inc_;
						*fdata = last_;
						fdata += stride;
					}

					count_ += nFrames;
					last_ = outputFrames_(nFrames - 1, 0u);
				}
			}

			if(*fdata != *fdata) {
				LOG(NLOG_ERROR, "NaN detected.");
			}

			//Mono source, so need to fill out channels if necessary.
			outputFrames_.FillChannels();
		}

		//Generator setters.
		inline void
		RampedValue_::setValueGen(ControlGenerator value) {
			valueGen_ = value;
		}

		inline void
		RampedValue_::setTargetGen(ControlGenerator target) {
			targetGen_ = target;
		}

		inline void
		RampedValue_::setLengthGen(ControlGenerator length) {
			lengthGen_ = length;
		}

		//Internal State setters.
		inline void
		RampedValue_::updateValue(float value) {
			//When the value gen changes, immediately abort the current ramp and go to the new value.
			finished_ = true;
			last_ = value;
			target_ = value;
			inc_ = 0.0f;
			count_ = 0ul;
		}

		inline void
		RampedValue_::updateTarget(float target, unsigned long lengthSamp) {
			target_ = target;
			count_ = 0ul;

			//Fixes bug with NaN when using Accelerate ramps.
			if(lengthSamp == 0ul || target_ == last_) {
				last_ = target_;
				finished_ = true;
				inc_ = 0.0f;
			}
			else {
				len_ = lengthSamp;
				inc_ = (float)(target_ - last_) / len_;
				finished_ = false;
			}

			if(inc_ != inc_) {
				LOG(NLOG_ERROR, "NaN found.");
			}
		}
	}

	class RampedValue : public TemplatedGenerator<NAudio_DSP::RampedValue_> {
	public:
		RampedValue(float startValue = 0.0f, float initLength = 0.05f) {
			target(startValue);
			value(startValue);
			length(initLength);
		}

		//Set target value. Changes to target gen input will create a new ramp from current value to target over the current length.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(RampedValue, target, setTargetGen);

		//Set length before reaching target value, in ms. Changes to length gen input will create a new ramp from current value to target over the provided length.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(RampedValue, length, setLengthGen);

		//Go to value immediately. Changes to the value gen input will abort the current ramp and go immediately to the specified value. Output will remain steady until a new target or length is set.
		NAUDIO_MAKE_CTRL_GEN_SETTERS(RampedValue, value, setValueGen);

		bool
		isFinished();
	};
}