#pragma once

#include "NAudioCore.h"

//This is heavily inspired in STKFrames, of the STK++ Toolkit. See: https://ccrma.stanford.edu/software/stk/
namespace NAudio {
	class NAudioFrames {
	protected:
		float* data;
		float dataRate;

		size_t nFrames;
		unsigned int nChannels;

		size_t size;
		size_t bufferSize;

	public:
		NAudioFrames(unsigned int nFrames = 0, unsigned int nChannels = 0);
		NAudioFrames(const float& value, unsigned int nFrames, unsigned int nChannels);
		NAudioFrames(NAudioFrames& f);
		virtual ~NAudioFrames();
		
		NAudioFrames& operator=(NAudioFrames& f);
		
		//The result can be used as an lvalue. This reference is valid until the resize function is called or the array is destroyed.
		//The index n must be between 0 and size less one. No range checking is performed unless DEBUG is defined.
		float& operator[](size_t n);
		float operator[](size_t n) const;		//TODO: See if it can be removed.

		//The dimensions of the argument are expected to be the same as self. No range checking is performed unless DEBUG is defined.
		void operator+=(NAudioFrames& f);
		void operator-=(NAudioFrames& f);
		void operator*=(NAudioFrames& f);
		void operator/=(NAudioFrames& f);
		
		//The result can be used as an lvalue. This reference is valid until the resize function is called or the array is destroyed.
		//The frame index must be between 0 and frames() - 1.
		//The channel index must be between 0 and channels() - 1. No range checking is performed unless DEBUG is defined.
		float& operator()(size_t frame, unsigned int channel);
		float operator()(size_t frame, unsigned int channel) const;		//TODO: See if it can be removed.
      
		//The src and dst indices must be between 0 and channels() - 1, and should not be the same number. No range checking is performed (yet).
		void
		CopyChannel(unsigned int src, unsigned int dst);
		
		//Fill all channels with contents of channel 0.
		void
		FillChannels();
    
		//Clear the frames data.
		void
		Clear();
    
		//Fill frames from other source. Copies channels from one object to another. Frame count must match.
		//If source has more channels than destination, they will be averaged.
		//If destination has more channels than source, they will be copied to all channels.
		void
		Copy(NAudioFrames& f);
        
		//Return an interpolated value at the fractional frame index and channel. This function performs linear interpolation. 
		//The frame index must be between 0.0 and frames() - 1. The \c channel index must be between 0 and channels() - 1. No range checking is performed unless DEBUG is defined.
		float
		Interpolate(float frame, unsigned int channel = 0);
		
		//Returns the total number of audio samples represented by the object.
		size_t
		Size() {
			return(size);
		}; 
		
		//Returns true if the object size is zero and false otherwise.
		bool
		Empty();
		
		//Resize self to represent the specified number of channels and frames. Changes the size of self based on the number of frames and channels.
		//No element assignment is performed. No memory deallocation occurs if the new size is smaller than the previous size.
		//Further, no new memory is allocated when the new size is smaller or equal to a previously allocated size.
		void
		Resize(size_t nFrames, unsigned int nChannels = 1);
		
		//Resize self to represent the specified number of channels and frames and perform element initialization.
		//Changes the size of self based on the number of frames and channels, and assigns value to every element.
		//No memory deallocation occurs if the new size is smaller than the previous size.
		//Further, no new memory is allocated when the new size is smaller or equal to a previously allocated size.
		void
		Resize(size_t nFrames, unsigned int nChannels, float value);
    
		//Resize and stretch/shrink existing data to fit new size.
		void
		Resample(size_t nFrames, unsigned int nChannels);
		
		//Return the number of channels represented by the data.
		inline unsigned int
		Channels() {
			return(nChannels);
		}
		
		//Return the number of sample frames represented by the data.
		inline unsigned long
		Frames() {
			return((unsigned long)nFrames);
		}
		
		//Set the sample rate associated with the NAudioFrames data. By default, this value is set equal to the current STK sample rate at the time of instantiation.
		void
		SetDataRate(float rate) {
			dataRate = rate;
		}
		
		//Return the sample rate associated with the NAudioFrames data. By default, this value is set equal to the current STK sample rate at the time of instantiation.
		float
		DataRate() {
			return(dataRate);
		}
	};
	
	inline bool
	NAudioFrames::Empty() {
		return(size > 0) ? false : true;
	}
	
	inline float&
	NAudioFrames::operator[](size_t n) {
		if(n >= size) {
			LOG(NLOG_ERROR, "Invalid index [%d] value! ", n);
		}

		return(data[n]);
	}
	
	inline float
	NAudioFrames::operator[](size_t n) const {
		if(n >= size) {
			LOG(NLOG_ERROR, "Invalid index [%d] value!", n);
		}
		
		return(data[n]);
	}
	
	inline float&
	NAudioFrames::operator()(size_t frame, unsigned int channel) {
		if(frame >= nFrames || channel >= nChannels) {
			LOG(NLOG_ERROR, "Invalid frame (%d) or channel (%u) value!", frame, channel);
		}
		
		return(data[frame * nChannels + channel]);
	}
	
	inline float
	NAudioFrames::operator()(size_t frame, unsigned int channel) const {
		if(frame >= nFrames || channel >= nChannels) {
			LOG(NLOG_ERROR, "Invalid frame (%d) or channel (%u) value!", frame, channel);
		}
		
		return(data[frame * nChannels + channel]);
	}
	
	inline void
	NAudioFrames::CopyChannel(unsigned int src, unsigned int dst) {
		float* sptr = data + src;
		float* dptr = data + dst;
		unsigned int stride = nChannels;
    
		VectorCpy(dptr, stride, sptr, stride, (int)nFrames);
	}
	
	inline void
	NAudioFrames::FillChannels() {
		for(unsigned int i = 1; i < nChannels; ++i) {
			this->CopyChannel(0, i);
		}
	}
  
	inline void
	NAudioFrames::Clear() {
		memset(data, 0, size * sizeof(float));
	}
  
	inline void
	NAudioFrames::Copy(NAudioFrames& f) {
		if(f.Frames() != nFrames) {
			LOG(NLOG_ERROR, "Frames argument must be of equal dimensions!");
		}
    
		unsigned int fChannels = f.Channels();
		
		float* dptr = data;
		float* fptr = &f[0];
    
		if(nChannels == fChannels) {
			memcpy(dptr, fptr, size * sizeof(float));
		}
		else if(nChannels < fChannels) {
			//Sum channels.
			memset(dptr, 0, size * sizeof(float));
			
			for(unsigned int c = 0; c < fChannels; ++c) {
				dptr = data;
				fptr = &f(0, c);
				
				for(unsigned int i = 0; i < nFrames; ++i, dptr += nChannels, fptr += fChannels) {
					*dptr += *fptr;
				}
			}
      
			//Apply scaling (average of channels).
			float s = 1.0f / fChannels;
			dptr = data;
			
			for(unsigned int i = 0; i < nFrames; ++i, dptr += nChannels) {
				*dptr *= s;
			}
		}
		else {
			//Just copy one channel, then fill.
			VectorCpy(dptr, nChannels, fptr, fChannels, (unsigned int)nFrames);
      
			//Fill all channels if necessary.
			FillChannels();
		}
	}
	
	inline void
	NAudioFrames::operator+=(NAudioFrames& f) {
		if(f.Frames() != nFrames) {
			LOG(NLOG_ERROR, "Frames argument must be of equal dimensions!");
		}
    
		float* fptr = &f[0];
		float* dptr = data;
    
		unsigned int fChannels = f.Channels();
    
		if(nChannels == fChannels) {
			for(unsigned int i = 0; i < size; ++i) {
				*dptr++ += *fptr++;
			}
		}
		else if(nChannels < fChannels) {
			//Just add first channel of rhs.
			for(unsigned int i = 0; i < nFrames; ++i) {
				*dptr++ -= *fptr++;
				fptr++;
			}
		}
		else {
			//Add rhs to both channels.
			for(unsigned int i = 0; i < nFrames; ++i) {
				*dptr++ += *fptr;
				*dptr++ += *fptr++;
			}
		}
	}
	
	inline void
	NAudioFrames::operator-=(NAudioFrames& f) {
		if(f.Frames() != nFrames) {
			LOG(NLOG_ERROR, "Frames argument must be of equal dimensions!");
		}
		
		float* fptr = &f[0];
		float* dptr = data;
		
		unsigned int fChannels = f.Channels();
    
		if(nChannels == fChannels) {
			for(unsigned int i = 0; i < size; ++i) {
				*dptr++ -= *fptr++;
			}
		}
		else if(nChannels < fChannels) {
			//Just subtract first channel of rhs.
			for(unsigned int i = 0; i < nFrames; ++i) {
				*dptr++ -= *fptr++;
				fptr++;
			}
		}
		else {
			//Subtract both channels by rhs.
			for(unsigned int i = 0; i < nFrames; ++i) {
				*dptr++ -= *fptr;
				*dptr++ -= *fptr++;
			}
		}
	}
  
	inline void
	NAudioFrames::operator*=(NAudioFrames& f) {
		if(f.Frames() != nFrames) {
			LOG(NLOG_ERROR, "Frames argument must be of equal dimensions!");
		}
    
		float* fptr = &f[0];
		float* dptr = data;
		
		unsigned int fChannels = f.Channels();
    
		if(nChannels == fChannels) {
			for(unsigned int i = 0; i < size; ++i) {
				*dptr++ *= *fptr++;
			}
		}
		else if(nChannels < fChannels) {
			//Just multiply by first channel of rhs.
			for(unsigned int i = 0; i < nFrames; ++i) {
				*dptr++ *= *fptr++;
				fptr++;
			}
		}
		else {
			//Multiply both channels by rhs.
			for(unsigned int i = 0; i < nFrames; ++i) {
				*dptr++ *= *fptr;
				*dptr++ *= *fptr++;
			}
		}
	}
	
	inline void
	NAudioFrames::operator/=(NAudioFrames& f) {
		if(f.Frames() != nFrames) {
			LOG(NLOG_ERROR, "Frames argument must be of equal dimensions!");
		}
    
		float* fptr = &f[0];
		float* dptr = data;
    
		unsigned int fChannels = f.Channels();
    
		if(nChannels == fChannels) {
			for(unsigned int i = 0; i < size; ++i) {
				*dptr++ /= *fptr++;
			}
		}
		else if(nChannels < fChannels) {
			//Just multiply by first channel of rhs.
			for(unsigned int i = 0; i < nFrames; ++i) {
				*dptr++ /= *fptr++;
				fptr++;
			}
		}
		else {
			//Multiply both channels by rhs.
			for(unsigned int i = 0; i < nFrames; ++i) {
				*dptr++ /= *fptr;
				*dptr++ /= *fptr++;
			}
		}
	}
}