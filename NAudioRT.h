//NAudioRT provides a common API (Application Programming Interface) for realtime audio input/output across Linux (native ALSA, Jack, and OSS), Macintosh OS X (CoreAudio and Jack),
//and Windows (DirectSound and ASIO) operating systems.
//See: http://www.music.mcgill.ca/~gary/rtaudio/

#pragma once

#include "NAudioError.h"

#include <string>
#include <vector>

static const std::string VERSION("4.0.12");

//NAudioRT data format type.
//Support for signed integers and floats. Audio data fed to/from an NAudioRT stream is assumed to always be in host byte order.
//The internal routines will automatically take care of any necessary byte-swapping between the host format and the soundcard. Thus, endian-ness is not a concern in the following format definitions:
//NAUDIORT_SINT8:		8-bit signed integer.
//NAUDIORT_SINT16:		16-bit signed integer.
//NAUDIORT_SINT24:		24-bit signed integer.
//NAUDIORT_SINT32:		32-bit signed integer.
//NAUDIORT_FLOAT32:		Normalized between plus/minus 1.0.
//NAUDIORT_FLOAT64:		Normalized between plus/minus 1.0.
typedef unsigned long NAudioRTFormat;

static const NAudioRTFormat NAUDIORT_SINT8 = 0x1;		//8-bit signed integer.
static const NAudioRTFormat NAUDIORT_SINT16 = 0x2;		//16-bit signed integer.
static const NAudioRTFormat NAUDIORT_SINT24 = 0x4;		//24-bit signed integer.
static const NAudioRTFormat NAUDIORT_SINT32 = 0x8;		//32-bit signed integer.
static const NAudioRTFormat NAUDIORT_FLOAT32 = 0x10;	//Normalized between plus/minus 1.0.
static const NAudioRTFormat NAUDIORT_FLOAT64 = 0x20;	//Normalized between plus/minus 1.0.

//NAudioRT stream option flags.
//The following flags can be OR'ed together to allow a client to make changes to the default stream behavior:
//NAUDIORT_NONINTERLEAVED:		Use non-interleaved buffers (default = interleaved).
//NAUDIORT_MINIMIZE_LATENCY:	Attempt to set stream parameters for lowest possible latency.
//NAUDIORT_HOG_DEVICE:			Attempt grab device for exclusive use.
//NAUDIORT_ALSA_USE_DEFAULT:	Use the "default" PCM device (ALSA only).
//
//By default, NAudioRT streams pass and receive audio data from the client in an interleaved format. By passing the NAUDIORT_NONINTERLEAVED flag to the openStream() function,
//audio data will instead be presented in non-interleaved buffers. In this case, each buffer argument in the NAudioRTCallback function will point to a single array of data,
//with nFrames samples for each channel concatenated back-to-back.
//For example, the first sample of data for the second channel would be located at index nFrames (assuming the buffer pointer was recast to the correct data type for the stream).
//
//Certain audio APIs offer a number of parameters that influence the I/O latency of a stream.
//By default, NAudioRT will attempt to set these parameters internally for robust (glitch-free) performance (though some APIs, like Windows Direct Sound, make this difficult).
//By passing the NAUDIORT_MINIMIZE_LATENCY flag to the openStream() function, internal stream settings will be influenced in an attempt to minimize stream latency, though possibly at the expense of stream performance.
//
//If the NAUDIORT_HOG_DEVICE flag is set, NAudioRT will attempt to open the input and/or output stream device(s) for exclusive use. Note that this is not possible with all supported audio APIs.
//If the NAUDIORT_SCHEDULE_REALTIME flag is set, NAudioRT will attempt to select realtime scheduling (round-robin) for the callback thread.
//If the NAUDIORT_ALSA_USE_DEFAULT flag is set, NAudioRT will attempt to open the "default" PCM device when using the ALSA API. Note that this will override any specified input or output device id.
typedef unsigned int NAudioRTStreamFlags;

static const NAudioRTStreamFlags NAUDIORT_NONINTERLEAVED = 0x1;			//Use non-interleaved buffers (default = interleaved).
static const NAudioRTStreamFlags NAUDIORT_MINIMIZE_LATENCY = 0x2;		//Attempt to set stream parameters for lowest possible latency.
static const NAudioRTStreamFlags NAUDIORT_HOG_DEVICE = 0x4;				//Attempt grab device and prevent use by others.
static const NAudioRTStreamFlags NAUDIORT_SCHEDULE_REALTIME = 0x8;		//Try to select realtime scheduling for callback thread.
static const NAudioRTStreamFlags NAUDIORT_ALSA_USE_DEFAULT = 0x10;		//Use the "default" PCM device (ALSA only).

//NAudioRT stream status (over- or underflow) flags.
//Notification of a stream over or underflow is indicated by a non-zero stream status argument in the NAudioRTCallback function. The stream status can be one of the following two options,
//depending on whether the stream is open for output and/or input:
//NAUDIORT_STREAM_MODE_INPUT_OVERFLOW:			Input data was discarded because of an overflow condition at the driver.
//NAUDIORT_STREAM_MODE_OUTPUT_UNDERFLOW:		The output buffer ran low, likely producing a break in the output sound.
typedef unsigned int NAudioRTStreamStatus;

static const NAudioRTStreamStatus NAUDIORT_STREAM_MODE_INPUT_OVERFLOW = 0x1;		//Input data was discarded because of an overflow condition at the driver.
static const NAudioRTStreamStatus NAUDIORT_STREAM_MODE_OUTPUT_UNDERFLOW = 0x2;		//The output buffer ran low, likely causing a gap in the output sound.

//NAudioRT callback function prototype.
//All NAudioRT clients must create a function of type NAudioRTCallback to read and/or write data from/to the audio stream. When the underlying audio system is ready for new input or output data,
//this function will be invoked.
//outputBuffer - For output (or duplex) streams, the client should write nFrames of audio sample frames into this buffer.
	//This argument should be recast to the datatype specified when the stream was opened. For input-only streams, this argument will be NULL.
//inputBuffer - For input (or duplex) streams, this buffer will hold nFrames of input audio sample frames.
	//This argument should be recast to the datatype specified when the stream was opened. For output-only streams, this argument will be NULL.
//nFrames - The number of sample frames of input or output data in the buffers. The actual buffer size in bytes is dependent on the data type and number of channels in use.
//streamTime - The number of seconds that have elapsed since the stream was started.
//status - If non-zero, this argument indicates a data overflow or underflow condition for the stream. The particular condition can be determined by comparison with the NAudioRTStreamStatus flags.
//userData - A pointer to optional data provided by the client when opening the stream (default = NULL).
//To continue normal stream operation, the NAudioRTCallback function should return a value of 0.
//To stop the stream and drain the output buffer, the function should return a value of 1.
//To abort the stream immediately, the client should return a value of 2.
typedef int(*NAudioRTCallback)(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime, NAudioRTStreamStatus status, void* userData);

//NAudioRT error callback function prototype.
//type - Type of error.
//errorText - Error description.
typedef void(*NAudioRTErrorCallback)(NAudioError::NAUDIO_EXCEPTION_TYPE type, const std::string& errorText);

//NAudioRT class declaration.
//NAudioRT is a "controller" used to select an available audio i/o interface. It presents a common API for the user to call but all functionality is implemented by the class RTApi and its subclasses.
//NAudioRT creates an instance of an RTApi subclass based on the user's API choice. If no choice is made, NAudioRT attempts to make a "logical" API selection.
class RTApi;

class NAudioRT {
public:
	//Audio API specifier arguments.
	enum NAUDIO_API {
		NAUDIO_API_UNSPECIFIED,		//Search for a working compiled API.
		NAUDIO_API_LINUX_ALSA,		//The Advanced Linux Sound Architecture API.
		NAUDIO_API_LINUX_PULSE,		//The Linux PulseAudio API.
		NAUDIO_API_LINUX_OSS,		//The Linux Open Sound System API.
		NAUDIO_API_UNIX_JACK,		//The Jack Low-Latency Audio Server API.
		NAUDIO_API_MACOSX_CORE,		//Macintosh OS-X Core Audio API.
		NAUDIO_API_WINDOWS_ASIO,	//The Steinberg Audio Stream I/O API.
		NAUDIO_API_WINDOWS_DS,		//The Microsoft Direct Sound API.
		NAUDIO_API_NAUDIORT_DUMMY	//A compilable but non-functional API.
	};

	//The public device information structure for returning queried values.
	struct DeviceInfo {
		NAudioRTFormat nativeFormats;				//Bit mask of supported data formats.

		std::vector<unsigned int> sampleRates;		//Supported sample rates (queried from list of standard rates).

		std::string name;							//Character string device identifier.

		unsigned int outputChannels;				//Maximum output channels supported by device.
		unsigned int inputChannels;					//Maximum input channels supported by device.
		unsigned int duplexChannels;				//Maximum simultaneous input/output channels supported by device.

		bool isDefaultOutput;						//True if this is the default output device. False if not.
		bool isDefaultInput;						//True if this is the default input device. False if not.
		bool probed;								//True if the device capabilities were successfully probed. False if not.

		DeviceInfo() :
			nativeFormats(NULL),
			outputChannels(0), inputChannels(0), duplexChannels(0),
			isDefaultOutput(false), isDefaultInput(false), probed(false)
		{
		}
	};

	//The structure for specifying input or ouput stream parameters.
	struct StreamParameters {
		unsigned int deviceId;			//Device index (0 to getDeviceCount() - 1).
		unsigned int nChannels;			//Number of channels.
		unsigned int firstChannel;		//First channel index on device (default = 0).

		StreamParameters() :
			deviceId(0u), nChannels(0u), firstChannel(0u)
		{
		}
	};

	//The structure for specifying stream options.
	//The following flags can be OR'ed together to allow a client to make changes to the default stream behavior:
	//NAUDIORT_NONINTERLEAVED:			Use non-interleaved buffers (default = interleaved).
	//NAUDIORT_MINIMIZE_LATENCY:		Attempt to set stream parameters for lowest possible latency.
	//NAUDIORT_HOG_DEVICE:				Attempt grab device for exclusive use.
	//NAUDIORT_SCHEDULE_REALTIME:		Attempt to select realtime scheduling for callback thread.
	//NAUDIORT_ALSA_USE_DEFAULT:		Use the "default" PCM device (ALSA only).
	//By default, NAudioRT streams pass and receive audio data from the client in an interleaved format. By passing the NAUDIORT_NONINTERLEAVED flag to the openStream() function,
	//audio data will instead be presented in non-interleaved buffers. In this case, each buffer argument in the NAudioRTCallback function will point to a single array of data,
	//with nFrames samples for each channel concatenated back-to-back. For example, the first sample of data for the second channel would be located at index nFrames
	//(assuming the buffer pointer was recast to the correct data type for the stream).
	//
	//Certain audio APIs offer a number of parameters that influence the I/O latency of a stream. By default, NAudioRT will attempt to set these parameters internally for robust (glitch-free) performance
	//(though some APIs, like Windows Direct Sound, make this difficult). By passing the NAUDIORT_MINIMIZE_LATENCY flag to the openStream() function, internal stream settings will be influenced in an attempt
	//to minimize stream latency, though possibly at the expense of stream performance.
	//
	//If the NAUDIORT_HOG_DEVICE flag is set, NAudioRT will attempt to open the input and/or output stream device(s) for exclusive use. Note that this is not possible with all supported audio APIs.
	//If the NAUDIORT_SCHEDULE_REALTIME flag is set, NAudioRT will attempt to select realtime scheduling (round-robin) for the callback thread.
	//The priority parameter will only be used if the NAUDIORT_SCHEDULE_REALTIME flag is set. It defines the thread's realtime priority.
	//If the NAUDIORT_ALSA_USE_DEFAULT flag is set, NAudioRT will attempt to open the "default" PCM device when using the ALSA API. Note that this will override any specified input or output device id.
	//The numberOfBuffers parameter can be used to control stream latency in the Windows DirectSound, Linux OSS, and Linux Alsa APIs only. A value of two is usually the smallest allowed.
	//Larger numbers can potentially result in more robust stream performance, though likely at the cost of stream latency.
	//The value set by the user is replaced during execution of the NAudioRT::openStream() function by the value actually used by the system.
	//
	//The \c streamName parameter can be used to set the client name when using the Jack API. By default, the client name is set to RTApiJack.
	//However, if you wish to create multiple instances of NAudioRT with Jack, each instance must have a unique client name.
	struct StreamOptions {
		NAudioRTStreamFlags flags;		//A bit-mask of stream flags (NAUDIORT_NONINTERLEAVED, NAUDIORT_MINIMIZE_LATENCY, NAUDIORT_HOG_DEVICE, NAUDIORT_ALSA_USE_DEFAULT).

		std::string streamName;			//A stream name (currently used only in Jack).

		unsigned int numberOfBuffers;	//Number of stream buffers.

		int priority;					//Scheduling priority of callback thread (only used with flag NAUDIORT_SCHEDULE_REALTIME).

		StreamOptions() :
			flags(0),
			numberOfBuffers(0u),
			priority(0)
		{
		}
	};

	//A static function to determine the current NAudioRT version.
	static std::string
	getVersion() {
		return(VERSION);
	}

	//A static function to determine the available compiled audio APIs. The values returned in the std::vector can be compared against the enumerated list values.
	//Note that there can be more than one API compiled for certain operating systems.
	static void
	getCompiledApi(std::vector<NAudioRT::NAUDIO_API>& apis) throw();

	//The constructor performs minor initialization tasks. No exceptions can be thrown.
	//If no API argument is specified and multiple API support has been compiled, the default order of use is JACK, ALSA, OSS (Linux systems) and ASIO, DS (Windows systems).
	NAudioRT(NAudioRT::NAUDIO_API api = NAUDIO_API_UNSPECIFIED) throw();

	//If a stream is running or open, it will be stopped and closed automatically.
	~NAudioRT() throw();

	//Returns the audio API specifier for the current instance of NAudioRT.
	NAudioRT::NAUDIO_API
	getCurrentApi() throw();

	//Performs a system query of available devices each time it is called, thus supporting devices connected after instantiation. If a system error occurs during processing, a warning will be issued.
	unsigned int
	getDeviceCount() throw();

	//Returns an NAudioRT::DeviceInfo structure for a specified device number.
	//Any device integer between 0 and getDeviceCount() - 1 is valid. If an invalid argument is provided, an NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) will be thrown.
	//If a device is busy or otherwise unavailable, the structure member "probed" will have a value of "false" and all other members are undefined.
	//If the specified device is the current default input or output device, the corresponding "isDefault" member will have a value of "true".
	NAudioRT::DeviceInfo
	getDeviceInfo(unsigned int device);

	//A function that returns the index of the default output device.
	//If the underlying audio API does not provide a "default device", or if no devices are available, the return value will be 0.
	//Note that this is a valid device identifier and it is the client's responsibility to verify that a device is available before attempting to open a stream.
	unsigned int
	getDefaultOutputDevice() throw();

	//A function that returns the index of the default input device.
	//If the underlying audio API does not provide a "default device", or if no devices are available, the return value will be 0.
	//Note that this is a valid device identifier and it is the client's responsibility to verify that a device is available before attempting to open a stream.
	unsigned int
	getDefaultInputDevice() throw();

	//A public function for opening a stream with the specified parameters.
	//An NAudioError (type = NAUDIO_EXCEPTION_TYPE_SYSTEM_ERROR) is thrown if a stream cannot be opened with the specified parameters or an error occurs during processing.
	//An NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) is thrown if any invalid device ID or channel number parameters are specified.
	//outputParameters - Specifies output stream parameters to use when opening a stream, including a device ID, number of channels, and starting channel number.
		//For input-only streams, this argument should be NULL. The device ID is an index value between 0 and getDeviceCount() - 1.
	//inputParameters - Specifies input stream parameters to use when opening a stream, including a device ID, number of channels, and starting channel number.
		//For output-only streams, this argument should be NULL. The device ID is an index value between 0 and getDeviceCount() - 1.
	//format - An NAudioRTFormat specifying the desired sample data format.
	//sampleRate - The desired sample rate (sample frames per second).
	//bufferFrames - A pointer to a value indicating the desired internal buffer size in sample frames. The actual value used by the device is returned via the same pointer.
		//A value of zero can be specified, in which case the lowest allowable value is determined.
	//callback - A client-defined function that will be invoked when input data is available and/or output data is needed.
	//userData - An optional pointer to data that can be accessed from within the callback function.
	//options - An optional pointer to a structure containing various global stream options,
		//including a list of OR'ed NAudioRTStreamFlags and a suggested number of stream buffers that can be used to control stream latency.
		//More buffers typically result in more robust performance, though at a cost of greater latency.
		//If a value of zero is specified, a system-specific median value is chosen.
		//If the NAUDIORT_MINIMIZE_LATENCY flag bit is set, the lowest allowable value is used. The actual value used is returned via the structure argument. The parameter is API dependent.
	//errorCallback - A client-defined function that will be invoked when an error has occured.
	void
	openStream(NAudioRT::StreamParameters* outputParameters, NAudioRT::StreamParameters* inputParameters, NAudioRTFormat format,
			   unsigned int sampleRate, unsigned int* bufferFrames, NAudioRTCallback callback, void* userData = NULL, NAudioRT::StreamOptions* options = NULL, NAudioRTErrorCallback errorCallback = NULL);

	//A function that closes a stream and frees any associated stream memory. If a stream is not open, this function issues a warning and returns (no exception is thrown).
	void
	closeStream() throw();

	//A function that starts a stream.
	//An NAudioError (type = NAUDIO_EXCEPTION_TYPE_SYSTEM_ERROR) is thrown if an error occurs during processing.
	//An NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) is thrown if a stream is not open.
	//A warning is issued if the stream is already running.
	void
	startStream();

	//Stop a stream, allowing any samples remaining in the output queue to be played.
	//An NAudioError (type = NAUDIO_EXCEPTION_TYPE_SYSTEM_ERROR) is thrown if an error occurs during processing.
	//An NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) is thrown if a stream is not open.
	//A warning is issued if the stream is already stopped.
	void
	stopStream();

	//Stop a stream, discarding any samples remaining in the input/output queue.
	//An NAudioError (type = NAUDIO_EXCEPTION_TYPE_SYSTEM_ERROR) is thrown if an error occurs during processing.
	//An NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) is thrown if a stream is not open.
	//A warning is issued if the stream is already stopped.
	void
	abortStream();

	//Returns true if a stream is open and false if not.
	bool
	isStreamOpen() const throw();

	//Returns true if the stream is running and false if it is stopped or not open.
	bool
	isStreamRunning() const throw();

	//Returns the number of elapsed seconds since the stream was started. If a stream is not open, an NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) will be thrown.
	double
	getStreamTime();

	//Returns the internal stream latency in sample frames. The stream latency refers to delay in audio input and/or output caused by internal buffering by the audio system and/or hardware.
	//For duplex streams, the returned value will represent the sum of the input and output latencies.
	//If a stream is not open, an NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) will be thrown. If the API does not report latency, the return value will be zero.
	long
	getStreamLatency();

	//Returns actual sample rate in use by the stream. On some systems, the sample rate used may be slightly different than that specified in the stream parameters.
	//If a stream is not open, an NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) will be thrown.
	unsigned int
	getStreamSampleRate();

	//Specify whether warning messages should be printed to stderr.
	void
	showWarnings(bool value = true) throw();

protected:
	void
	openRTApi(NAudioRT::NAUDIO_API api);

	RTApi* rtapi_;
};

//Operating system dependent thread functionality.
#if defined(__WINDOWS_DS__) || defined(__WINDOWS_ASIO__)
	#include <windows.h>
	#include <process.h>

	typedef unsigned long ThreadHandle;
	typedef CRITICAL_SECTION StreamMutex;
#elif defined(__LINUX_ALSA__) || defined(__LINUX_PULSE__) || defined(__UNIX_JACK__) || defined(__LINUX_OSS__) || defined(__MACOSX_CORE__)
	//Using pthread library for various flavors of unix.
	#include <pthread.h>

	typedef pthread_t ThreadHandle;
	typedef pthread_mutex_t StreamMutex;
#else //Setup for "dummy" behavior.
	#define __NAUDIORT_DUMMY__
	typedef int ThreadHandle;
	typedef int StreamMutex;
#endif

//This global structure type is used to pass callback information between the private NAudioRT stream structure and global callback handling functions.
struct CallbackInfo {
	void* object;			//Used as a "this" pointer.

	ThreadHandle thread;

	void* callback;
	void* userData;
	void* errorCallback;
	void* apiInfo;			//void pointer for API specific callback information.

	bool isRunning;
	bool doRealtime;

	int priority;

	CallbackInfo() :
		object(NULL), callback(NULL), userData(NULL), errorCallback(NULL), apiInfo(NULL),
		isRunning(false), doRealtime(false)
	{
	}
};

#pragma pack(push, 1)

class S24 {
protected:
	unsigned char c3[3];

public:
	S24() {
	}

	S24&
	operator=(const int& i) {
		c3[0] = (i & 0x000000ff);
		c3[1] = (i & 0x0000ff00) >> 8;
		c3[2] = (i & 0x00ff0000) >> 16;

		return(*this);
	}

	S24(const S24& v) {
		*this = v;
	}

	S24(const double& d) {
		*this = (int)d;
	}

	S24(const float& f) {
		*this = (int)f;
	}

	S24(const signed short& s) {
		*this = (int)s;
	}

	S24(const char& c) {
		*this = (int)c;
	}

	int
	asInt() {
		int i = c3[0] | (c3[1] << 8) | (c3[2] << 16);

		if(i & 0x800000) {
			i |= ~0xffffff;
		}

		return(i);
	}
};

#pragma pack(pop)

#if defined(HAVE_GETTIMEOFDAY)
#include <sys/time.h>
#endif

#include <sstream>

//RTApi class declaration. Subclasses of RTApi contain all API- and OS-specific code necessary to fully implement the NAudioRT API.
//Note that RTApi is an abstract base class and cannot be explicitly instantiated. The class NAudioRT will create an instance of an RTApi subclass (RTApiOss, RTApiAlsa, RTApiJack, RTApiCore, RTApiDs, or RTApiAsio).
class RTApi {
public:
	RTApi();
	virtual ~RTApi();

	virtual NAudioRT::NAUDIO_API
	getCurrentApi() = 0;

	virtual unsigned int
	getDeviceCount() = 0;

	virtual NAudioRT::DeviceInfo
	getDeviceInfo(unsigned int device) = 0;

	virtual unsigned int
	getDefaultInputDevice();

	virtual unsigned int
	getDefaultOutputDevice();

	void
	openStream(NAudioRT::StreamParameters* outputParameters, NAudioRT::StreamParameters* inputParameters, NAudioRTFormat format,
			   unsigned int sampleRate, unsigned int* bufferFrames, NAudioRTCallback callback, void* userData, NAudioRT::StreamOptions* options, NAudioRTErrorCallback errorCallback);

	virtual void
	closeStream();

	virtual void
	startStream() = 0;

	virtual void
	stopStream() = 0;

	virtual void
	abortStream() = 0;

	long
	getStreamLatency();

	unsigned int
	getStreamSampleRate();

	virtual double
	getStreamTime();

	bool
	isStreamOpen() const {
		return(stream_.state != STREAM_STATE_CLOSED);
	}

	bool
	isStreamRunning() const {
		return(stream_.state == STREAM_STATE_RUNNING);
	}

	void
	showWarnings(bool value) {
		showWarnings_ = value;
	}

protected:
	static const unsigned int MAX_SAMPLE_RATES;
	static const unsigned int SAMPLE_RATES[];

	enum {
		NAUDIO_FAILURE,
		NAUDIO_SUCCESS
	};

	enum StreamState {
		STREAM_STATE_STOPPED,
		STREAM_STATE_STOPPING,
		STREAM_STATE_RUNNING,
		STREAM_STATE_CLOSED = -50
	};

	enum StreamMode {
		STREAM_MODE_OUTPUT,
		STREAM_MODE_INPUT,
		STREAM_MODE_DUPLEX,
		STREAM_MODE_UNINITIALIZED = -75
	};

	//A protected structure used for buffer conversion.
	struct ConvertInfo {
		int channels;
		int inJump;
		int outJump;

		NAudioRTFormat inFormat;
		NAudioRTFormat outFormat;

		std::vector<int> inOffset;
		std::vector<int> outOffset;
	};

	//A protected structure for audio streams.
	struct RTApiStream {
		unsigned int device[2];					//Playback and record, respectively.

		void* apiHandle;						//void pointer for API specific stream handle information

		StreamMode mode;						//STREAM_MODE_OUTPUT, STREAM_MODE_INPUT, or STREAM_MODE_DUPLEX.
		StreamState state;						//STOPPED, RUNNING, or CLOSED.

		char* userBuffer[2];					//Playback and record, respectively.
		char* deviceBuffer;

		bool doConvertBuffer[2];				//Playback and record, respectively.
		bool userInterleaved;
		bool deviceInterleaved[2];				//Playback and record, respectively.
		bool doByteSwap[2];						//Playback and record, respectively.

		unsigned int sampleRate;
		unsigned int bufferSize;
		unsigned int nBuffers;

		unsigned int nUserChannels[2];			//Playback and record, respectively.
		unsigned int nDeviceChannels[2];		//Playback and record channels, respectively.
		unsigned int channelOffset[2];			//Playback and record, respectively.
		unsigned long latency[2];				//Playback and record, respectively.

		NAudioRTFormat userFormat;
		NAudioRTFormat deviceFormat[2];			//Playback and record, respectively.

		StreamMutex mutex;
		CallbackInfo callbackInfo;
		ConvertInfo convertInfo[2];

		double streamTime;						//Number of elapsed seconds since the stream started.

#if defined(HAVE_GETTIMEOFDAY)
		struct timeval lastTickTimestamp;
#endif

		RTApiStream() :
			apiHandle(NULL), deviceBuffer(NULL)
		{
			device[0] = 11111u;
			device[1] = 11111u;
		}
	};

	typedef signed short Int16;
	typedef S24 Int24;
	typedef signed int Int32;
	typedef float Float32;
	typedef double Float64;

	std::ostringstream errorStream_;
	std::string errorText_;

	bool showWarnings_;

	RTApiStream stream_;

	//Protected, api-specific method that attempts to open a device with the given parameters. This function must be implemented by all subclasses.
	//If an error is encountered during the probe, a "warning" message is reported and NAUDIO_FAILURE is returned. A successful probe is indicated by a return value of NAUDIO_SUCCESS.
	virtual bool
	probeDeviceOpen(unsigned int device, StreamMode mode, unsigned int channels, unsigned int firstChannel, unsigned int sampleRate, NAudioRTFormat format, unsigned int* bufferSize, NAudioRT::StreamOptions* options);

	//A protected function used to increment the stream time.
	void
	tickStreamTime();

	//Protected common method to clear an RTApiStream structure.
	void
	clearStreamInfo();

	//Protected common method that throws an NAudioError (type = NAUDIO_EXCEPTION_TYPE_INVALID_USE) if a stream is not open.
	void
	verifyStream();

	//Protected common error method to allow global control over error handling.
	void
	error(NAudioError::NAUDIO_EXCEPTION_TYPE type);

	//Protected method used to perform format, channel number, and/or interleaving conversions between the user and device buffers.
	void
	convertBuffer(char* outBuffer, char* inBuffer, ConvertInfo& info);

	//Protected common method used to perform byte-swapping on buffers.
	void
	byteSwapBuffer(char* buffer, unsigned int samples, NAudioRTFormat format);

	//Protected common method that returns the number of bytes for a given format.
	unsigned int
	formatBytes(NAudioRTFormat format);

	//Protected common method that sets up the parameters for buffer conversion.
	void
	setConvertInfo(StreamMode mode, unsigned int firstChannel);
};

//Inline NAudioRT definitions.
inline NAudioRT::NAUDIO_API
NAudioRT::getCurrentApi() throw() {
	return(rtapi_->getCurrentApi());
}

inline unsigned int
NAudioRT::getDeviceCount() throw() {
	return(rtapi_->getDeviceCount());
}

inline NAudioRT::DeviceInfo
NAudioRT::getDeviceInfo(unsigned int device) {
	return(rtapi_->getDeviceInfo(device));
}

inline unsigned int
NAudioRT::getDefaultInputDevice() throw() {
	return(rtapi_->getDefaultInputDevice());
}

inline unsigned int
NAudioRT::getDefaultOutputDevice() throw() {
	return(rtapi_->getDefaultOutputDevice());
}

inline void
NAudioRT::closeStream() throw() {
	return(rtapi_->closeStream());
}

inline void
NAudioRT::startStream() {
	return(rtapi_->startStream());
}

inline void
NAudioRT::stopStream() {
	return(rtapi_->stopStream());
}

inline void
NAudioRT::abortStream() {
	return(rtapi_->abortStream());
}

inline bool
NAudioRT::isStreamOpen() const throw() {
	return(rtapi_->isStreamOpen());
}

inline bool
NAudioRT::isStreamRunning() const throw() {
	return(rtapi_->isStreamRunning());
}

inline long
NAudioRT::getStreamLatency() {
	return(rtapi_->getStreamLatency());
}

inline unsigned int
NAudioRT::getStreamSampleRate() {
	return(rtapi_->getStreamSampleRate());
}

inline double
NAudioRT::getStreamTime() {
	return(rtapi_->getStreamTime());
}

inline void
NAudioRT::showWarnings(bool value) throw() {
	rtapi_->showWarnings(value);
}

//RTApi Subclass prototypes.
#if defined(__MACOSX_CORE__)
	#include <CoreAudio/AudioHardware.h>

	class RTApiCore : public RTApi {
	public:
		RTApiCore();
		~RTApiCore();

		NAudioRT::NAUDIO_API
		getCurrentApi() {
			return(NAudioRT::NAUDIO_API_MACOSX_CORE);
		}

		unsigned int
		getDeviceCount();

		NAudioRT::DeviceInfo
		getDeviceInfo(unsigned int device);

		unsigned int
		getDefaultOutputDevice();

		unsigned int
		getDefaultInputDevice();

		void
		closeStream();

		void
		startStream();

		void
		stopStream();

		void
		abortStream();

		long
		getStreamLatency();

		//This function is intended for internal use only. It must be public because it is called by the internal callback handler, which is not a member of NAudioRT.
		//External use of this function will most likely produce highly undesireable results!
		bool
		callbackEvent(AudioDeviceID deviceId, const AudioBufferList* inBufferList, const AudioBufferList* outBufferList);

	private:
		bool
		probeDeviceOpen(unsigned int device, StreamMode mode, unsigned int channels, unsigned int firstChannel, unsigned int sampleRate,
						NAudioRTFormat format, unsigned int* bufferSize, NAudioRT::StreamOptions* options);

		static const char*
		getErrorCode(OSStatus code);
	};
#endif

#if defined(__UNIX_JACK__)
	class RTApiJack : public RTApi {
	public:
		RTApiJack();
		~RTApiJack();

		NAudioRT::NAUDIO_API
		getCurrentApi() {
			return(NAudioRT::NAUDIO_API_UNIX_JACK);
		}

		unsigned int
		getDeviceCount();

		NAudioRT::DeviceInfo
		getDeviceInfo(unsigned int device);

		void
		closeStream();

		void
		startStream();

		void
		stopStream();

		void
		abortStream();

		long
		getStreamLatency();

		//This function is intended for internal use only. It must be public because it is called by the internal callback handler, which is not a member of NAudioRT.
		//External use of this function will most likely produce highly undesireable results!
		bool
		callbackEvent(unsigned long nframes);

	private:
		bool
		probeDeviceOpen(unsigned int device, StreamMode mode, unsigned int channels, unsigned int firstChannel, unsigned int sampleRate,
						NAudioRTFormat format, unsigned int* bufferSize, NAudioRT::StreamOptions* options);
	};
#endif

#if defined(__WINDOWS_ASIO__)
	class RTApiAsio : public RTApi {
	public:
		RTApiAsio();
		~RTApiAsio();

		NAudioRT::NAUDIO_API
		getCurrentApi() {
			return(NAudioRT::NAUDIO_API_WINDOWS_ASIO);
		}

		unsigned int
		getDeviceCount();

		NAudioRT::DeviceInfo
		getDeviceInfo(unsigned int device);

		void
		closeStream();

		void
		startStream();

		void
		stopStream();

		void
		abortStream();

		long
		getStreamLatency();

		//This function is intended for internal use only. It must be public because it is called by the internal callback handler, which is not a member of NAudioRT.
		//External use of this function will most likely produce highly undesireable results!
		bool
		callbackEvent(long bufferIndex);

	private:
		std::vector<NAudioRT::DeviceInfo> devices_;

		bool coInitialized_;

		void
		saveDeviceInfo();

		bool
		probeDeviceOpen(unsigned int device, StreamMode mode, unsigned int channels, unsigned int firstChannel, unsigned int sampleRate,
						NAudioRTFormat format, unsigned int* bufferSize, NAudioRT::StreamOptions* options);
	};
#endif

#if defined(__WINDOWS_DS__)
	class RTApiDs : public RTApi {
	public:
		RTApiDs();
		~RTApiDs();

		NAudioRT::NAUDIO_API
		getCurrentApi() {
			return(NAudioRT::NAUDIO_API_WINDOWS_DS);
		}

		unsigned int
		getDeviceCount();

		unsigned int
		getDefaultOutputDevice();

		unsigned int
		getDefaultInputDevice();

		NAudioRT::DeviceInfo
		getDeviceInfo(unsigned int device);

		void
		closeStream();

		void
		startStream();

		void
		stopStream();

		void
		abortStream();

		long
		getStreamLatency();

		//This function is intended for internal use only. It must be public because it is called by the internal callback handler, which is not a member of NAudioRT.
		//External use of this function will most likely produce highly undesireable results!
		void
		callbackEvent();

	private:
		std::vector<struct DsDevice> dsDevices;

		long duplexPrerollBytes;

		bool coInitialized_;
		bool buffersRolling;

		bool
		probeDeviceOpen(unsigned int device, StreamMode mode, unsigned int channels, unsigned int firstChannel, unsigned int sampleRate,
						NAudioRTFormat format, unsigned int* bufferSize, NAudioRT::StreamOptions *options);
	};
#endif

#if defined(__LINUX_ALSA__)
	class RTApiAlsa : public RTApi {
	public:
		RTApiAlsa();
		~RTApiAlsa();

		NAudioRT::NAUDIO_API
		getCurrentApi() {
			return(NAudioRT::NAUDIO_API_LINUX_ALSA);
		}

		unsigned int
		getDeviceCount();

		NAudioRT::DeviceInfo
		getDeviceInfo(unsigned int device);

		void
		closeStream();

		void
		startStream();

		void
		stopStream();

		void
		abortStream();

		//This function is intended for internal use only. It must be public because it is called by the internal callback handler, which is not a member of NAudioRT.
		//External use of this function will most likely produce highly undesireable results!
		void
		callbackEvent();

	private:
		std::vector<NAudioRT::DeviceInfo> devices_;

		void
		saveDeviceInfo();

		bool
		probeDeviceOpen(unsigned int device, StreamMode mode, unsigned int channels, unsigned int firstChannel, unsigned int sampleRate,
						NAudioRTFormat format, unsigned int* bufferSize, NAudioRT::StreamOptions* options);
	};
#endif

#if defined(__LINUX_PULSE__)
	class RTApiPulse : public RTApi {
	public:
		~RTApiPulse();

		NAudioRT::NAUDIO_API
		getCurrentApi() {
			return(NAudioRT::NAUDIO_API_LINUX_PULSE);
		}

		unsigned int
		getDeviceCount();

		NAudioRT::DeviceInfo
		getDeviceInfo(unsigned int device);

		void
		closeStream();

		void
		startStream();

		void
		stopStream();

		void
		abortStream();

		//This function is intended for internal use only. It must be public because it is called by the internal callback handler, which is not a member of NAudioRT.
		//External use of this function will most likely produce highly undesireable results!
		void
		callbackEvent();

	private:
		std::vector<NAudioRT::DeviceInfo> devices_;

		void
		saveDeviceInfo();

		bool
		probeDeviceOpen(unsigned int device, StreamMode mode, unsigned int channels, unsigned int firstChannel, unsigned int sampleRate,
						NAudioRTFormat format, unsigned int* bufferSize, NAudioRT::StreamOptions* options);
	};
#endif

#if defined(__LINUX_OSS__)
	class RTApiOss : public RTApi {
	public:
		RTApiOss();
		~RTApiOss();

		NAudioRT::NAUDIO_API
		getCurrentApi() {
			return(NAudioRT::NAUDIO_API_LINUX_OSS);
		}

		unsigned int
		getDeviceCount();

		NAudioRT::DeviceInfo
		getDeviceInfo(unsigned int device);

		void
		closeStream();

		void
		startStream();

		void
		stopStream();

		void
		abortStream();

		// This function is intended for internal use only.  It must be
		// public because it is called by the internal callback handler,
		// which is not a member of NAudioRT.  External use of this function
		// will most likely produce highly undesireable results!
		void
		callbackEvent();

	private:
		bool
		probeDeviceOpen(unsigned int device, StreamMode mode, unsigned int channels, unsigned int firstChannel, unsigned int sampleRate,
							 NAudioRTFormat format, unsigned int* bufferSize, NAudioRT::StreamOptions* options);
	};
#endif

#if defined(__NAUDIORT_DUMMY__)
	class RTApiDummy : public RTApi {
	public:
		RTApiDummy() {
			errorText_ = "RTApiDummy: This class provides no functionality.";
			error(NAudioError::NAUDIO_EXCEPTION_TYPE_NAUDIO_EXCEPTION_TYPE_WARNING);
		}

		NAudioRT::NAUDIO_API
		getCurrentApi() {
			return(NAudioRT::NAUDIO_API_NAUDIORT_DUMMY);
		}

		unsigned int
		getDeviceCount() {
			return(0);
		}

		NAudioRT::DeviceInfo
		getDeviceInfo(unsigned int /*device*/) {
			NAudioRT::DeviceInfo info;
			return(info);
		}

		void
		closeStream() {
		}

		void
		startStream() {
		}

		void
		stopStream() {
		}

		void
		abortStream() {
		}

	private:
		bool
		probeDeviceOpen(unsigned int /*device*/, StreamMode /*mode*/, unsigned int /*channels*/, unsigned int /*firstChannel*/, unsigned int /*sampleRate*/,
						NAudioRTFormat /*format*/, unsigned int* /*bufferSize*/, NAudioRT::StreamOptions* /*options*/)
		{
			return(false);
		}
	};
#endif