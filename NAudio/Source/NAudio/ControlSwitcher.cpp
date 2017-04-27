#include "ControlSwitcher.h"

namespace NAudio {
	namespace NAudio_DSP {
		ControlSwitcher_::ControlSwitcher_() :
			inputIndex_(ControlValue(0.0f)), doesWrap_(ControlValue(0.0f)), addAfterWrap_(ControlValue(0.0f)),
			lastInputIndex_(-1), lastIndexOutputValue(-1)
		{
		}

		inline void
		ControlSwitcher_::computeOutput(const SynthesisContext_& context) {
			if(inputs_.size() > 0) {
				output_.triggered = false;

				//First, get the default input.
				ControlGeneratorOutput indexOutput = inputIndex_.tick(context);

				//Check to see if the value changed. If it hasn't changed, ignore it, lest we overwrite a value set by one of the triggers.
				if(lastIndexOutputValue != indexOutput.value) {
					lastIndexOutputValue = indexOutput.value;
					currentInputIndex_ = indexOutput.value;
				}

				//Then check the input triggers to see if one fired.
				for(std::map<int, ControlGenerator>::iterator it = triggers_.begin(); it != triggers_.end(); ++it) {
					ControlGeneratorOutput  triggerOut = it->second.tick(context);
					if(triggerOut.triggered) {
						currentInputIndex_ = it->first;
					}
				}

				ControlGeneratorOutput doesWrapOut = doesWrap_.tick(context);
				ControlGeneratorOutput addAfterWrapOut = addAfterWrap_.tick(context);

				int cleanedInput = doesWrapOut.value ? currentInputIndex_ % inputs_.size() : Clamp(currentInputIndex_, 0, (int)(inputs_.size() - 1));

				int index = 0;
				for(std::vector<ControlGenerator>::iterator it = inputs_.begin(); it != inputs_.end(); ++it) {
					ControlGeneratorOutput tempOut = it->tick(context);

					if(index++ == cleanedInput) {
						output_ = tempOut;
					}
				}

				if(doesWrapOut.value) {
					//Do the add after wrap thing. Mostly useful for going to scale degrees in higher octaves.
					int numTimes = currentInputIndex_ / (int)(inputs_.size());
					output_.value += numTimes * addAfterWrapOut.value;
				}

				if(lastInputIndex_ != currentInputIndex_) {
					lastInputIndex_ = currentInputIndex_;
					output_.triggered = true;
				}
			}
		}

		void
		ControlSwitcher_::addInput(ControlGenerator input) {
			inputs_.push_back(input);
		}

		void
		ControlSwitcher_::setInputs(std::vector<ControlGenerator> inputs) {
			inputs_ = inputs;
		}

		void
		ControlSwitcher_::setDoesWrap(ControlGenerator doesWrap) {
			doesWrap_ = doesWrap;
		}

		void
		ControlSwitcher_::setInputIndex(ControlGenerator inputIndexArg) {
			inputIndex_ = inputIndexArg;
		}

		void
		ControlSwitcher_::setAddAfterWrap(ControlGenerator addAfterWrap) {
			addAfterWrap_ = addAfterWrap;
		}

		void
		ControlSwitcher_::setTriggerForIndex(ControlGenerator trigger, int index) {
			triggers_[index] = trigger;
		}
	}

	ControlSwitcher&
	ControlSwitcher::triggerForIndex(ControlGenerator trigger, int index) {
		gen()->setTriggerForIndex(trigger, index);
		return(*this);
	}

	ControlSwitcher&
	ControlSwitcher::setFloatInputs(std::vector<float> inputs) {
		std::vector<ControlGenerator> newInputs;

		for(std::vector<float>::iterator it = inputs.begin(); it != inputs.end(); ++it) {
			newInputs.push_back(ControlValue(*it));
		}

		gen()->setInputs(newInputs);

		return(*this);
	}
}