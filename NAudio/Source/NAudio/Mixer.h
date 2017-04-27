#pragma once

#include "Synth.h"
#include "CompressorLimiter.h"

using std::vector;

namespace NAudio {
	namespace NAudio_DSP {
		//A mixer is like an adder but acts as a source and allows dynamic removal.
		class Mixer_ : public BufferFiller_ {
		private:
			NAudioFrames workSpace_;
			std::vector<BufferFiller> inputs_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			Mixer_();

			void
			addInput(BufferFiller input);

			void
			removeInput(BufferFiller input);
		};

		inline void
		Mixer_::computeSynthesisBlock(const SynthesisContext_& context) {
			//Clear buffer.
			outputFrames_.Clear();

			//Tick and add inputs.
			for(unsigned int i = 0; i < inputs_.size(); ++i) {
				//Tick each bufferFiller every time, with our context (for now).
				inputs_[i].tick(workSpace_, context);
				outputFrames_ += workSpace_;
			}
		}
	}

	class Mixer : public TemplatedBufferFiller<NAudio_DSP::Mixer_> {
	public:
		void
		addInput(BufferFiller input) {
			gen()->lockMutex();
			gen()->addInput(input);
			gen()->unlockMutex();
		}

		void
		removeInput(BufferFiller input) {
			gen()->lockMutex();
			gen()->removeInput(input);
			gen()->unlockMutex();
		}
	};
}