#pragma once

#include "NAudioFrames.h"

namespace NAudio {
	//Tonicframes subclass with ability to tick in and out. Allows random-access/multi-tap/etc.
	class DelayLine : public NAudioFrames {
	private:
		bool isInitialized_;
		bool interpolates_;

		long writeHead_;
		float readHead_;
		float lastDelayTime_;

	public:
		//Allocation parameters are binding. No post-allocation resizing or modifying channel layout (for now anyway). Samples are interleaved if allocated with multiple channels.
		DelayLine();

		//MUST be called prior to usage!
		void
		initialize(float maxDelay = 1.0f, unsigned int channels = 1u);

		//Set whether interpolates or not.
		void
		setInterpolates(bool doesInterpolate) {
			interpolates_ = doesInterpolate;
		}

		//Zero delay line.
		void
		clear();

		//The below functions are single-sample and very one-purposed for a reason: As a helper class this will allow the most flexibility for feedback and more complex delay structures such as comb filters, etc.
		//Return one interpolated, delayed sample. Does not advance read/write head. NOTE: No bounds checking on channel index. Improper use will result in access outside array bounds.
		inline float
		tickOut(float delayTime, unsigned int channel = 0u) {
			if(delayTime != lastDelayTime_) {
				float dSamp = Clamp(delayTime * NAudio::SampleRate(), 0.0f, (float)nFrames);
				readHead_ = (float)writeHead_ - dSamp;

				if(readHead_ < 0) {
					readHead_ += (float)nFrames;
				}

				lastDelayTime_ = delayTime;
			}

			if(interpolates_) {
				//Fractional and integral part of read head.
				float fidx;
				float frac = modff(readHead_, &fidx);

				int idx_a = ((int)fidx * nChannels + channel);
				
				//This happens occasionally due to floating point rounding.
				if(idx_a >= (int)size) {
					idx_a -= (int)size;
				}

				int idx_b = idx_a + nChannels;

				if(idx_b >= (int)size) {
					idx_b -= (int)size;
				}

				//Linear interpolation.
				return(data[idx_a] + frac * (data[idx_b] - data[idx_a]));
			}
			else {
				return((data[((int)(readHead_)) * nChannels + channel]));
			}
		}

		//Tick one sample in (write at write head). Does not advance read/write head. NOTE: No bounds checking on channel index. Improper use will result in access outside array bounds.
		inline void
		tickIn(float sample, unsigned int channel = 0u) {
			data[writeHead_ * nChannels + channel] = sample;
		}

		//Advance read/write heads.
		inline void
		advance() {
			if(++writeHead_ >= (long)nFrames) {
				writeHead_ = 0;
			}

			//Assume read head advances by 1 for efficiency in constant-delay lines.
			readHead_ += 1.0f;

			if(readHead_ >= nFrames) {
				readHead_ -= (float)nFrames;
			}
		}
	};
}