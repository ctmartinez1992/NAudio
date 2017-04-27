#include "BufferPlayer.h"
#include "ControlTrigger.h"

namespace NAudio {
	namespace NAudio_DSP {
		BufferPlayer_::BufferPlayer_() :
			currentSample(0), isFinished_(true) {
			doesLoop_ = ControlValue(false);
			trigger_ = ControlTrigger();
			startPosition_ = ControlValue(0);
		}

		BufferPlayer_::~BufferPlayer_() {
		}

		void
		BufferPlayer_::setBuffer(SampleTable buffer) {
			buffer_ = buffer;
			setIsStereoOutput(buffer.channels() == 2u);
			samplesPerSynthesisBlock = kSynthesisBlockSize * buffer_.channels();
		}

		inline void
		BufferPlayer_::computeSynthesisBlock(const SynthesisContext_& context) {
			bool doesLoop = doesLoop_.tick(context).value;
			bool trigger = trigger_.tick(context).triggered;
			float startPosition = startPosition_.tick(context).value;

			if(trigger) {
				isFinished_ = false;
				currentSample = startPosition * SampleRate() * buffer_.channels();
			}

			if(isFinished_) {
				outputFrames_.Clear();
			}
			else {
				int samplesLeftInBuf = (int)buffer_.size() - currentSample;
				int samplesToCopy = Min(samplesPerSynthesisBlock, samplesLeftInBuf);

				copySamplesToOutputBuffer(currentSample, samplesToCopy);

				if(samplesToCopy < samplesPerSynthesisBlock) {
					if(doesLoop) {
						currentSample = 0;
					}
					else {
						isFinished_ = true;
					}
				}
				else {
					currentSample += samplesPerSynthesisBlock;
				}
			}
		}
	}
}