#pragma once

#include "NAudioFrames.h"

namespace NAudio {
	namespace NAudio_DSP {
		class SampleTable_ {
		protected:
			NAudioFrames frames_;

		public:
			SampleTable_(unsigned int frames, unsigned int channels);

			//Property getters.
			unsigned int
			channels() {
				return(frames_.Channels());
			}

			unsigned long
			frames() {
				return(frames_.Frames());
			}

			size_t
			size() {
				return(frames_.Size());
			}

			//Pointer to start of data array.
			float*
			dataPointer() {
				return(&frames_[0]);
			}

			void
			resize(unsigned int frames, unsigned int channels) {
				frames_.Resize(frames, channels);
			}

			void
			resample(unsigned int frames, unsigned int channels) {
				frames_.Resample(frames, channels);
			}
		};
	}

	//Access to a persistent NAudioFrames instance, so the same audio data can be read from multiple places.
	class SampleTable : public NSmartPointer<NAudio_DSP::SampleTable_> {
	public:
		SampleTable(unsigned int nFrames = 64, unsigned int nChannels = 2) :
			NSmartPointer<NAudio_DSP::SampleTable_>(new NAudio_DSP::SampleTable_(nFrames, nChannels))
		{
		}

		//Property getters.
		unsigned int
		channels() const {
			return(obj->channels());
		}

		unsigned long
		frames() const {
			return(obj->frames());
		}

		size_t
		size() const {
			return(obj->size());
		}

		//Pointer to start of data array.
		float*
		dataPointer() {
			return(obj->dataPointer());
		}

		void
		resize(unsigned int frames, unsigned int channels) {
			obj->resize(frames, channels);
		}

		void
		resample(unsigned int frames, unsigned int channels) {
			obj->resample(frames, channels);
		}
	};
}