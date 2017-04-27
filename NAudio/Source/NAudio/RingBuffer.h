#pragma once

#include "Effect.h"
#include "SampleTable.h"

namespace NAudio {
	namespace NAudio_DSP {
		class RingBuffer_ : public SampleTable_ {
		private:
			unsigned long readHead_;
			unsigned long writeHead_;

		public:
			RingBuffer_(unsigned int frames, unsigned int channels);

			void
			write(float* data, unsigned int nFrames, unsigned int nChannels);

			void
			read(NAudioFrames& outFrames);

			void
			reset();
		};

		inline void
		RingBuffer_::write(float* data, unsigned int nFrames, unsigned int nChannels) {
			//Detect overrun.
			unsigned long finalWriteHead = (writeHead_ + nFrames) % frames();

			if(finalWriteHead >= readHead_ && (writeHead_ < readHead_ || finalWriteHead < writeHead_)) {
				LOG(NLOG_WARN, "RingBuffer overrun detected.");
			}

			float* writeptr = &frames_(writeHead_, 0);

			unsigned int bufChannels = channels();
			unsigned long bufFrames = frames();

			if(bufChannels == nChannels) {
				for(unsigned int i = 0; i < nFrames; ++i) {
					for(unsigned int c = 0; c < nChannels; ++c) {
						*writeptr++ = *data++;
					}

					if(++writeHead_ >= bufFrames) {
						writeHead_ = 0ul;
						writeptr = &frames_[0];
					}
				}
			}
			else if(bufChannels > nChannels) {
				for(unsigned int i = 0; i < nFrames; ++i) {
					for(unsigned int c = 0; c < bufChannels; ++c) {
						*writeptr++ = *data;
					}

					data++;

					if(++writeHead_ >= bufFrames) {
						writeHead_ = 0ul;
						writeptr = &frames_[0];
					}
				}
			}
			else {
				for(unsigned int i = 0; i < nFrames; ++i) {
					*writeptr = 0.0f;

					for(unsigned int c = 0; c < nChannels; ++c) {
						*writeptr += *data++;
					}

					*writeptr++ /= (float)nChannels;

					if(++writeHead_ >= bufFrames) {
						writeHead_ = 0ul;
						writeptr = &frames_[0];
					}
				}
			}
		}

		inline void
		RingBuffer_::read(NAudioFrames& outFrames) {
			//Detect underrun.
			unsigned long finalReadHead = (readHead_ + outFrames.Frames()) % frames();

			if(finalReadHead > writeHead_ && (readHead_ < writeHead_ || finalReadHead < readHead_)) {
				LOG(NLOG_WARN, "RingBuffer underrun detected.");
			}

			float* readptr = &frames_(readHead_, 0);
			float* outptr = &outFrames[0];

			unsigned long nFrames = outFrames.Frames();
			unsigned int nChannels = outFrames.Channels();
			unsigned long bufFrames = frames();
			unsigned int bufChannels = channels();

			if(bufChannels == nChannels) {
				for(unsigned int i = 0; i < nFrames; ++i) {
					for(unsigned int c = 0; c < nChannels; ++c) {
						*outptr++ = *readptr++;
					}

					if(++readHead_ >= bufFrames) {
						readHead_ = 0ul;
						readptr = &frames_[0];
					}
				}
			}
			else if(bufChannels > nChannels) {
				for(unsigned int i = 0; i < nFrames; ++i) {
					*outptr = 0;

					for(unsigned int c = 0; c < bufChannels; ++c) {
						*outptr += *readptr++;
					}

					*outptr /= (float)bufChannels;
					outptr++;

					if(++readHead_ >= bufFrames) {
						readHead_ = 0ul;
						readptr = &frames_[0];
					}
				}
			}
			else {
				for(unsigned int i = 0; i < nFrames; ++i) {
					for(unsigned int c = 0; c < nChannels; ++c) {
						*outptr++ = *readptr;
					}

					readptr++;

					if(++readHead_ >= bufFrames) {
						readHead_ = 0ul;
						readptr = &frames_[0];
					}
				}
			}
		}

		inline void
		RingBuffer_::reset() {
			readHead_ = 0ul;
			writeHead_ = 0ul;
		}
	}

	//Like a SampleTable_, but with counters for over/underrun detection and easy synchronous read/write.
	//TODO: Maybe should template the SampleTable smart pointer instead of statically casting the object?
	class RingBuffer : public SampleTable {
	public:
		RingBuffer(unsigned int nFrames = 64, unsigned int nChannels = 2) {
			delete obj;
			obj = new NAudio_DSP::RingBuffer_(nFrames, nChannels);
		}

		void
		write(float* data, unsigned int nFrames, unsigned int nChannels) {
			static_cast<NAudio_DSP::RingBuffer_*>(obj)->write(data, nFrames, nChannels);
		}

		void
		read(NAudioFrames& outFrames) {
			static_cast<NAudio_DSP::RingBuffer_*>(obj)->read(outFrames);
		}

		void
		reset() {
			static_cast<NAudio_DSP::RingBuffer_*>(obj)->reset();
		}
	};

	namespace NAudio_DSP {
		class RingBufferReader_ : public Generator_ {
		protected:
			RingBuffer ringBuffer_;

		public:
			void
			setRingBuffer(RingBuffer buffer) {
				setIsStereoOutput(buffer.channels() == 2u);
				ringBuffer_ = buffer;
			}

			void
			computeSynthesisBlock(const SynthesisContext_& context);
		};

		inline void
		RingBufferReader_::computeSynthesisBlock(const SynthesisContext_& context) {
			//Get some output from the table.
			ringBuffer_.read(outputFrames_);
		}

		//TODO: Maybe make this an Effect_?
		class RingBufferWriter_ {
		protected:
			RingBuffer ringBuffer_;
			std::string tableName_;

		public:
			~RingBufferWriter_();

			void
			initRingBuffer(std::string name, unsigned int nFrames, unsigned int nChannels);

			void
			write(float* data, unsigned int nFrames, unsigned int nChannels);

			void
			reset();
		};

		inline void
		RingBufferWriter_::write(float* data, unsigned int nFrames, unsigned int nChannels) {
			ringBuffer_.write(data, nFrames, nChannels);
		}
	}

	class RingBufferReader : public TemplatedGenerator<NAudio_DSP::RingBufferReader_> {
	public:
		RingBufferReader&
		bufferName(std::string name);
	};

	class RingBufferWriter : public NSmartPointer<NAudio_DSP::RingBufferWriter_> {
	public:
		RingBufferWriter() :
			NSmartPointer<NAudio_DSP::RingBufferWriter_>(new NAudio_DSP::RingBufferWriter_)
		{
		}

		RingBufferWriter(std::string name, unsigned int nFrames, unsigned int nChannels);

		void
		write(float* data, unsigned int nFrames, unsigned int nChannels) {
			obj->write(data, nFrames, nChannels);
		}

		void
		reset() {
			obj->reset();
		}
	};
}