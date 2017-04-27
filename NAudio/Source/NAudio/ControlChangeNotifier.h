#pragma once

#include "ControlGenerator.h"
#include "ControlConditioner.h"

namespace NAudio {
	//Abstract class to be extended by any class that wants to be notified of control events. 
	class ControlChangeSubscriber {
	public:
		virtual ~ControlChangeSubscriber() {
		}

		virtual void
		valueChanged(std::string, float) = 0;
	};

	namespace NAudio_DSP {
		class ControlChangeNotifier_ : public ControlConditioner_ {
		protected:
			std::vector<ControlChangeSubscriber*> subscribers;

			ControlGeneratorOutput outputToSendToUI;
			bool outputReadyToBeSentToUI;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			std::string name;

			ControlChangeNotifier_();
			~ControlChangeNotifier_();

			void
			addValueChangedSubscriber(ControlChangeSubscriber* sub);
			void
			removeValueChangedSubscriber(ControlChangeSubscriber* sub);
			void
			sendControlChangesToSubscribers();
		};
	}

	//A ControlChangeNotifier observes a ControlGenerator and notifies its subscribers when its value changes, or when the value of its outputs "triggered" flag is true.
	//Generally you don't instantiate this object directly, rather you "publish" a controlGenerator using Synth::publishChanges(myControlGen, "nameOfMyControlGen");.
	class ControlChangeNotifier : public TemplatedControlConditioner<NAudio::ControlChangeNotifier, NAudio_DSP::ControlChangeNotifier_> {
	public:
		void
		sendControlChangesToSubscribers();

		void
		addValueChangedSubscriber(ControlChangeSubscriber* resp) {
			gen()->addValueChangedSubscriber(resp);
		}

		void
		removeValueChangedSubscriber(ControlChangeSubscriber* sub) {
			gen()->removeValueChangedSubscriber(sub);
		}

		void
		setName(std::string name) {
			gen()->name = name;
		}
	};
}