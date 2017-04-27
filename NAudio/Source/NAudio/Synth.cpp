#include "Synth.h"

namespace NAudio {
	//Synth Factory.
	SynthFactory::map_type* SynthFactory::map;

	namespace NAudio_DSP {
		Synth_::Synth_() :
			limitOutput_(true)
		{
			limiter_.setIsStereo(true);
		}

		void
		Synth_::setParameter(std::string name, float value, bool normalized) {
			if(parameters_.find(name) != parameters_.end()) {
				ControlParameter& param = parameters_[name];

				if(normalized) {
					param.setNormalizedValue(value);
				}
				else {
					param.value(value);
				}
			}
			else {
				LOG(NLOG_ERROR, "Message: %s was not registered. You can register a message using Synth::addParameter.", name);
			}
		}

		ControlParameter
		Synth_::addParameter(std::string name, float initialValue) {
			if(parameters_.find(name) == parameters_.end()) {
				ControlParameter param = ControlParameter().name(name).value(initialValue).displayName(name);
				parameters_[name] = param;

				orderedParameterNames_.push_back(name);
			}

			return(parameters_[name]);
		}

		void
		Synth_::addParameter(ControlParameter parameter) {
			std::string name = parameter.getName();
			parameters_[name] = parameter;

			orderedParameterNames_.push_back(name);
		}

		void
		Synth_::addParametersFromSynth(Synth synth) {
			std::vector<ControlParameter> params = synth.getParameters();

			for(unsigned int i = 0; i < params.size(); ++i) {
				addParameter(params[i]);
			}
		}

		std::vector<ControlParameter>
		Synth_::getParameters() {
			std::vector<ControlParameter> returnParams;

			for(std::vector<std::string>::iterator it = orderedParameterNames_.begin(); it != orderedParameterNames_.end(); ++it) {
				std::map<std::string, ControlParameter>::iterator paramIt = parameters_.find(*it);

				if(paramIt != parameters_.end()) {
					returnParams.push_back(paramIt->second);
				}
			}

			return(returnParams);
		}

		void
		Synth_::sendControlChangesToSubscribers() {
			std::vector<ControlChangeNotifier>::iterator it = controlChangeNotifiersList_.begin();

			for(; it != controlChangeNotifiersList_.end(); ++it) {
				it->sendControlChangesToSubscribers();
			}
		}

		void
		Synth_::addControlChangeSubscriber(std::string name, ControlChangeSubscriber* resp) {
			if(controlChangeNotifiers_.find(name) != controlChangeNotifiers_.end()) {
				controlChangeNotifiers_[name].addValueChangedSubscriber(resp);
			}
			else {
				LOG(NLOG_ERROR, "No value called %s was exposed to the UI.", name);
			}
		}

		void
		Synth_::addControlChangeSubscriber(ControlChangeSubscriber* sub) {
			for(std::vector<ControlChangeNotifier>::iterator it = controlChangeNotifiersList_.begin(); it != controlChangeNotifiersList_.end(); ++it) {
				it->addValueChangedSubscriber(sub);
			}
		}

		void
		Synth_::removeControlChangeSubscriber(ControlChangeSubscriber* sub) {
			for(std::vector<ControlChangeNotifier>::iterator it = controlChangeNotifiersList_.begin(); it != controlChangeNotifiersList_.end(); ++it) {
				it->removeValueChangedSubscriber(sub);
			}
		}

		ControlChangeNotifier
		Synth_::publishChanges(ControlGenerator input, std::string name) {
			ControlChangeNotifier messenger;

			messenger.setName(name);
			messenger.input(input);

			controlChangeNotifiersList_.push_back(messenger);

			if(name != "") {
				controlChangeNotifiers_[name] = messenger;
			}

			addAuxControlGenerator(messenger);

			return(messenger);
		}
	}
}