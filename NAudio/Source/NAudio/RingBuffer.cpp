#include "RingBuffer.h"

namespace NAudio {
	//Registry of ring buffers.
	NDictionary<RingBuffer> s_RingBuffers_;

	namespace NAudio_DSP {
		RingBuffer_::RingBuffer_(unsigned int frames, unsigned int channels) :
			SampleTable_(frames, channels),
			writeHead_(0ul), readHead_(0ul)
		{
		}

		RingBufferWriter_::~RingBufferWriter_() {
			s_RingBuffers_.RemoveObjectNamed(tableName_);
		}

		void
		RingBufferWriter_::initRingBuffer(std::string name, unsigned int nFrames, unsigned int nChannels) {
			RingBuffer table = RingBuffer(nFrames, nChannels);

			//Overwrite existing entry if there is one.
			if(s_RingBuffers_.ContainsObjectNamed(name)) {
				LOG(NLOG_WARN, "There is already a ring buffer named %s open for writing. It will be replaced with this one.", name);
			}

			s_RingBuffers_.InsertObject(name, table);

			ringBuffer_ = table;
		}

		void
		RingBufferWriter_::reset() {
			ringBuffer_.reset();
		}
	}

	RingBufferReader&
	RingBufferReader::bufferName(std::string name) {
		if(s_RingBuffers_.ContainsObjectNamed(name)) {
			gen()->setRingBuffer(s_RingBuffers_.ObjectNamed(name));
		}
		else {
			LOG(NLOG_ERROR, "RingBuffer named %s does not exist.", name);
		}

		return(*this);
	}

	RingBufferWriter::RingBufferWriter(std::string name, unsigned int nFrames, unsigned int nChannels) {
		obj = new NAudio_DSP::RingBufferWriter_();
		obj->initRingBuffer(name, nFrames, nChannels);
	}
}