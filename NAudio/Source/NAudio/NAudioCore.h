#pragma once

#include "NUtilBase.h"

//Determine if C++11 is available. If not, some synths cannot be used.
#define NAUDIO_HAS_CPP_11 (__cplusplus > 199711L)

//Platform-specific macros and includes.
#if defined (__APPLE__)
	#import <Accelerate/Accelerate.h>
#endif

#if (defined(__APPLE__) || defined(__linux__))
	#include <pthread.h> 
	#define NAUDIO_MUTEX_T				pthread_mutex_t
	#define NAUDIO_MUTEX_INIT(x)		pthread_mutex_init(&x, NULL)
	#define NAUDIO_MUTEX_DESTROY(x)		pthread_mutex_destroy(&x)
	#define NAUDIO_MUTEX_LOCK(x)		pthread_mutex_lock(&x)
	#define NAUDIO_MUTEX_UNLOCK(x)		pthread_mutex_unlock(&x)
#elif(defined(_WIN32) || defined(__WIN32__))
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	//Clear these macros to avoid interfering with ControlParameter::min and ControlParameter::max.
	#undef min
	#undef max

	#if(_MSC_VER < 1800)
		//Windows' C90 <cmath> header does not define log2.
		inline static float log2(float n) {
			return(log(n) / log(2));
		}
	#endif

	//Windows native mutexes.
	#define NAUDIO_MUTEX_T				CRITICAL_SECTION
	#define NAUDIO_MUTEX_INIT(x)		InitializeCriticalSection(&x)
	#define NAUDIO_MUTEX_DESTROY(x)		DeleteCriticalSection(&x)
	#define NAUDIO_MUTEX_LOCK(x)		EnterCriticalSection(&x)
	#define NAUDIO_MUTEX_UNLOCK(x)		LeaveCriticalSection(&x)
#endif

//Macro for enabling denormal rounding on audio thread.
//TODO: Any other non-SSE platforms that allow denormals by default? ARM-based targets (iPhone, for example) do not.
#if (defined(__SSE__) || defined(_WIN32))
	#include <xmmintrin.h>
	#define NAUDIO_ENABLE_DENORMAL_ROUNDING() _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON)
#else
	#define NAUDIO_ENABLE_DENORMAL_ROUNDING()
#endif

//Channel indices.
#define NAUDIO_LEFT				0
#define NAUDIO_RIGHT			1

//Causes 32nd bit in double to have fractional value 1 (decimal point on 32-bit word boundary).
//Allowing some efficient shortcuts for table lookup using power-of-two length tables.
#define BIT32DECPT				1572864.0

//Top-level namespace.
//Objects under the NAudio namespace are used to build synths and generator chains.
namespace NAudio {
	//DSP-level namespace.
	//Objects under the NAudio_DSP namespace are internal DSP-level objects not intended for public usage.
	namespace NAudio_DSP {
		static float sampleRate_ = 44100.f;   
	}
  
	//Global constants.
  
	//Set the operational sample rate.
	//NOTE: CHANGING WHILE RUNNING WILL RESULT IN UNDEFINED BEHAVIOR. MUST BE SET PRIOR TO OBJECT ALLOCATION.
	static void setSampleRate(float sampleRate) {
		NAudio_DSP::sampleRate_ = sampleRate;
	}
  
	//Return sample rate.
	static float SampleRate() {
		return(NAudio_DSP::sampleRate_);
	};
	
	//"Vector" size for audio processing. ControlGenerators update at this rate.
	//THIS VALUE SHOULD BE A POWER-OF-TWO WHICH IS LESS THAN THE HARDWARE BUFFER SIZE.
	static const unsigned int kSynthesisBlockSize = 64;
  
	//Global Types.
  
	//For fast computation of int/fract using some bit manipulation.
	union FastPhasor {
		double d;
		int i[2];
	};
  
	namespace NAudio_DSP {
		//Context which defines a particular synthesis graph.
		//Context passed down from root BufferFiller graph object to all sub-generators. Synchronizes signal flow in cases when generator output is shared between multiple inputs.
		struct SynthesisContext_ {
			unsigned long elapsedFrames;			//Number of frames elapsed since context started (unsigned long will last 38+ days before overflow at 44.1 kHz).
			double elapsedTime;						//Elapsed time since context started.
      
			//If true, generators will be forced to compute fresh output.
			//TODO: Not fully implmenented yet.
			bool forceNewOutput;
            
			SynthesisContext_() :
				elapsedFrames(0), elapsedTime(0), forceNewOutput(true)
			{
			}
			
			void tick() {
				elapsedFrames += kSynthesisBlockSize;
				elapsedTime = (double)elapsedFrames / SampleRate();
				
				forceNewOutput = false;
			}
		};
	}
  
	//Dummy context for ticking things in-place. Will always be at time 0, forceNewOutput is true.
	static const NAudio_DSP::SynthesisContext_ DummyContext;
  
	//Frequency / MIDI.
	//Midi note number to frequency in Hz.
	inline static float MtoF(float nn) {
		return(440.0f * powf(2.0f, (nn - 69.0f) / 12.0f));
	}
  
	//Frequency in Hz to midi note number.
	inline static float FtoM(float f) {
		return(12.0f * (logf(f / 440.0f) / logf(2.0f)) + 69.0f);
	}
  
	//Decibels.
	//Using 0 dBFS as 1.0f.
	inline static float LinTodB(float lv) {
		return(20.0f * log10f(Max(0.0f, lv)));
	}
  
	inline static float DBToLin(float dBFS) {
		return(powf(10.f, (dBFS / 20.0f)));
	}
	
	//May want to implement custom exception behavior here, but for now, this is essentially a typedef.
	class NAudioException : public std::runtime_error {
	public:
		NAudioException(std::string const& error) :
			std::runtime_error(error)
		{
		}
	};
}