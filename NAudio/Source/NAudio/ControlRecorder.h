#pragma once

#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlRecorder_ : public ControlConditioner_ {
		protected:
			std::vector<ControlGeneratorOutput> recording;
			std::vector<ControlGeneratorOutput>::iterator playbackHead;

			ControlGenerator mode;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlRecorder_();

			void
			setMode(ControlGenerator);
		};
	}

	class ControlRecorder : public TemplatedControlConditioner<ControlRecorder, NAudio_DSP::ControlRecorder_> {
	public:
		//TODO: What a shitty name for an enum, put it ControlRecorderMode or something...
		enum Mode {
			RECORD,
			PLAY,
			STOP
		};

		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlRecorder, mode, setMode)
	};

	//Put down here so we can use the enum.
	namespace NAudio_DSP {
		inline void
		ControlRecorder_::computeOutput(const SynthesisContext_& context) {
			ControlGeneratorOutput inputOut = input_.tick(context);
			ControlGeneratorOutput modeOut = mode.tick(context);

			ControlRecorder::Mode currentMode = (ControlRecorder::Mode)((int)modeOut.value);

			if(modeOut.triggered) {
				if(currentMode == ControlRecorder::STOP) {
					LOG(NLOG_INFO, "STOP");
					recording.clear();
				}
				else if(currentMode == ControlRecorder::PLAY) {
					LOG(NLOG_INFO, "PLAY");
					playbackHead = recording.begin();
				}
				else if(currentMode == ControlRecorder::RECORD) {
					LOG(NLOG_INFO, "RECORD");
					playbackHead = recording.begin();
					recording.clear();
				}
			}

			//Tmp.
			static int count = 0;

			switch(currentMode) {
				case ControlRecorder::RECORD:
					recording.push_back(inputOut);
					output_ = inputOut;

					break;

				case ControlRecorder::STOP:
					output_ = inputOut;

					break;

				case ControlRecorder::PLAY:
					output_ = *playbackHead;
					playbackHead++;
					count++;

					if(playbackHead >= recording.end()) {
						playbackHead = recording.begin();
						count = 0;
					}

					//printf("ControlRecorder_::computeOutput playing back sample: %i of %lu. Value is: %f\n", count, recording.size(), output_.value);

					break;

				default:
					break;
			}
		}
	}
}