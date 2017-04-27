#include "NAudioFrames.h"

namespace NAudio {
	NAudioFrames::NAudioFrames(unsigned int nFrames, unsigned int nChannels) :
		nFrames(nFrames), nChannels(nChannels)
	{
		if(nChannels > 2) {
			LOG(NLOG_ERROR, "Invalid number of channels. NFrames is limited to mono or stereo only (1 or 2 channels).");
		}
		
		size = nFrames * nChannels;
		bufferSize = size;
		
		if(size > 0) {
			data = (float*)calloc(size, sizeof(float));

			if(data == NULL) {
				LOG(NLOG_ERROR, "Memory allocation error in constructor!");
			}
		}
		else {
			data = 0;
		}
		
		dataRate = NAudio::SampleRate();
	}
	
	NAudioFrames::NAudioFrames(const float& value, unsigned int nFrames, unsigned int nChannels) :
		nFrames(nFrames), nChannels(nChannels)
	{
		if(nChannels > 2) {
			LOG(NLOG_ERROR, "Invalid number of channels. NFrames is limited to mono or stereo only (1 or 2 channels).");
		}
  
		size = nFrames * nChannels;
		bufferSize = size;
		
		if(size > 0) {
			data = (float*)malloc(size * sizeof(float));
			
			if(data == NULL) {
				LOG(NLOG_ERROR, "Memory allocation error in constructor!");
			}
			
			for(long i = 0; i < (long)size; ++i) {
				data[i] = value;
			}
		}
		else {
			data = 0;
		}
		
		dataRate = NAudio::SampleRate();
	}

	NAudioFrames::NAudioFrames(NAudioFrames& f) :
		size(0), bufferSize(0)
	{
		Resize(f.Frames(), f.Channels());
		dataRate = NAudio::SampleRate();

		for(unsigned int i = 0; i < size; ++i) {
			data[i] = f[i];
		}
	}

	NAudioFrames::~NAudioFrames() {
		if(data) {
			free(data);
		}
	}
	
	NAudioFrames&
	NAudioFrames::operator=(NAudioFrames& f) {
		size = 0;
		bufferSize = 0;
		
		Resize(f.Frames(), f.Channels());
		dataRate = NAudio::SampleRate();
		
		for(unsigned int i = 0; i < size; ++i) {
			data[i] = f[i];
		}
		
		return(*this);
	}
	
	void
	NAudioFrames::Resize(size_t nFrames, unsigned int nChannels) {
		if(nChannels > 2) {
			LOG(NLOG_ERROR, "Invalid number of channels. NFrames is limited to mono or stereo only (1 or 2 channels).");
		}

		if(this->nFrames != nFrames || this->nChannels != nChannels) {
			this->nFrames = nFrames;
			this->nChannels = nChannels;
			
			//Preserve as much of old data as we can.
			float* oldData = data;
			unsigned long oldSize = (unsigned long)size;

			size = nFrames * nChannels;

			if(size > bufferSize) {
				data = (float*)malloc(size * sizeof(float));

				if(oldData) {
					for(unsigned int i = 0; i < oldSize; ++i) {
						data[i] = oldData[i];
					}
				}
				
				if(data == NULL) {
					LOG(NLOG_ERROR, "Memory allocation error!");
				}
				
				bufferSize = size;
				
				if(oldData) {
					free(oldData);
				}
			}
		}
	}
	
	void
	NAudioFrames::Resize(size_t nFrames, unsigned int nChannels, float value) {
		this->Resize(nFrames, nChannels);

		for(size_t i = 0; i < size; ++i) {
			data[i] = value;
		}
	}
  
	void
	NAudioFrames::Resample(size_t nFrames, unsigned int nChannels) {
		if(nChannels > 2) {
			LOG(NLOG_ERROR, "Invalid number of channels. NFrames is limited to mono or stereo only (1 or 2 channels).");
		}
    
		if(nFrames != nFrames || nChannels != nChannels) {
			//Preserve as much of old data as we can.
			float* oldData = data;
			unsigned long oldFrames = (unsigned long)nFrames;
			unsigned int oldchannels = nChannels;
      
			nFrames = nFrames;
			nChannels = nChannels;
      
			size = nFrames * nChannels;
        
			data = (float*)malloc(size * sizeof(float));
      
			if(data == NULL) {
				LOG(NLOG_ERROR, "Memory allocation error!");
			}
			
			//Resample the content (brute-force, no AA applied).
			if(oldData) {
				float inc = (float)oldFrames / nFrames;
				
				for(unsigned int c = 0; c < nChannels; ++c) {
					float fIdx = 0.0f;

					for(unsigned int i = 0; i < nFrames; ++i) {
						float fi;
						float y1;
						float y2;
						
						float frac = modff(fIdx, &fi);
						unsigned int idx = (unsigned int)fi;
            
						unsigned int ocIdx = (oldchannels > 1) ? c : 0;
            
						if(idx == oldFrames - 1) {
							data[i * nChannels + c] = oldData[idx * oldchannels + ocIdx];
						}
						else {
							y1 = oldData[idx * oldchannels + ocIdx];
							y2 = oldData[(idx + 1) * oldchannels + ocIdx];
							data[i * nChannels + c] = y1 + frac * (y2 - y1);
						}
						
						//Handle different channel mapping.
						if(oldchannels > nChannels) {
							//Add and average.
							y1 = oldData[idx * oldchannels + ocIdx + 1];
							y2 = oldData[(idx+1) * oldchannels + ocIdx + 1];
							data[i * nChannels + c] += (y1 + frac * (y2 - y1));
							data[i * nChannels + c] *= 0.5f;
						}

						fIdx += inc;
					}
				}
			}

			bufferSize = size;
			
			if(oldData) {
				free(oldData);
			}
		}
	}
	
	float
	NAudioFrames::Interpolate(float frame, unsigned int channel) {
		if(frame < 0.0 || frame > (float) (nFrames - 1) || channel >= nChannels) {
			LOG(NLOG_ERROR, "Invalid frame (%f) or channel (%u) value!", frame, channel);
		}

		float output;
		size_t iIndex = (size_t)frame;					//Integer part of index.
		float alpha = frame - (float)iIndex;			//Fractional part of index.
		
		iIndex = iIndex * nChannels + channel;
		output = data[iIndex];
		
		if(alpha > 0.0f) {
			output += (alpha * (data[iIndex + nChannels] - output));
		}
		
		return(output);
	}
}