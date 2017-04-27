#pragma once

#include "Generator.h"

namespace NAudio {
	namespace NAudio_DSP {
		//Base class for any generator expected to produce output for a buffer fill. BufferFillers provide a high-level interface for combinations of generators, and can be used to fill large buffers.
		class BufferFiller_ : public Generator_ {
		private:
			unsigned long bufferReadPosition_;
			NAUDIO_MUTEX_T mutex_;

		protected:
			NAudio_DSP::SynthesisContext_ synthContext_;

		public:
			BufferFiller_();
			~BufferFiller_();

			//Mutex for swapping inputs, etc.
			void lockMutex();
			void unlockMutex();

			//Process a single synthesis vector, output to frames. Tick method without context argument passes down this instance's SynthesisContext_.
			void
			tick(NAudioFrames& frames);

			void
			fillBufferOfFloats(float* outData, unsigned int numFrames, unsigned int numChannels);
		};

		inline void
		BufferFiller_::lockMutex() {
			NAUDIO_MUTEX_LOCK(mutex_);
		}

		inline void
		BufferFiller_::unlockMutex() {
			NAUDIO_MUTEX_UNLOCK(mutex_);
		}

		inline void
		BufferFiller_::tick(NAudioFrames& frames) {
			lockMutex();
			{
				Generator_::tick(frames, synthContext_);
				synthContext_.tick();
			}
			unlockMutex();
		}

		inline void
		BufferFiller_::fillBufferOfFloats(float* outData, unsigned int numFrames, unsigned int numChannels) {
			//Flush denormals on this thread.
			NAUDIO_ENABLE_DENORMAL_ROUNDING();

			if(numChannels > outputFrames_.Channels()) {
				LOG(NLOG_ERROR, "Mismatch in channels sent to Synth::fillBufferOfFloats.");
			}

			const unsigned long sampleCount = (unsigned long)outputFrames_.Size();
			const unsigned int channelsPerSample = (outputFrames_.Channels() - numChannels) + 1u;

			float sample = 0.0f;
			float* outputSamples = &outputFrames_[bufferReadPosition_];

			for(unsigned int i = 0; i < numFrames * numChannels; ++i) {
				sample = 0;

				for(unsigned int c = 0; c < channelsPerSample; ++c) {
					if(bufferReadPosition_ == 0) {
						tick(outputFrames_);
					}

					sample += *outputSamples++;

					if(++bufferReadPosition_ == sampleCount) {
						bufferReadPosition_ = 0;
						outputSamples = &outputFrames_[0];
					}
				}

				*outData++ = sample / (float)channelsPerSample;
			}
		}
	}

	class BufferFiller : public Generator {
	public:
		BufferFiller(NAudio_DSP::BufferFiller_* newBf) :
			Generator(newBf)
		{
		}

		//Fill an arbitrarily-sized, interleaved buffer of audio samples as floats. This BufferFiller's outputGen is used to fill an interleaved buffer starting at outData.
		inline void
		fillBufferOfFloats(float* outData, unsigned int numFrames, unsigned int numChannels) {
			static_cast<NAudio_DSP::BufferFiller_*>(obj)->fillBufferOfFloats(outData, numFrames, numChannels);
		}
	};

	template<class GenType>
	class TemplatedBufferFiller : public BufferFiller {
	protected:
		GenType* gen() {
			return static_cast<GenType*>(obj);
		}
	public:
		TemplatedBufferFiller() :
			BufferFiller(new GenType)
		{
		}
	};
}