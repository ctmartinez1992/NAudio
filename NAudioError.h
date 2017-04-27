//Exception handling class for NAudioRT & RtMidi. The NAudioError class is quite simple but it does allow errors to be "caught" by NAudioError::Type.
//See the NAudioRT and RtMidi documentation to know which methods can throw an NAudioError.

#pragma once

#include <exception>
#include <iostream>
#include <string>

class NAudioError : public std::exception {
public:
	enum NAUDIO_EXCEPTION_TYPE {
		NAUDIO_EXCEPTION_TYPE_WARNING,				//A non-critical error.
		NAUDIO_EXCEPTION_TYPE_DEBUG_WARNING,		//A non-critical error which might be useful for debugging.
		NAUDIO_EXCEPTION_TYPE_UNSPECIFIED,			//The default, unspecified error type.
		NAUDIO_EXCEPTION_TYPE_NO_DEVICES_FOUND,		//No devices found on system.
		NAUDIO_EXCEPTION_TYPE_INVALID_DEVICE,		//An invalid device ID was specified.
		NAUDIO_EXCEPTION_TYPE_MEMORY_ERROR,			//An error occured during memory allocation.
		NAUDIO_EXCEPTION_TYPE_INVALID_PARAMETER,	//An invalid parameter was specified to a function.
		NAUDIO_EXCEPTION_TYPE_INVALID_USE,			//The function was called incorrectly.
		NAUDIO_EXCEPTION_TYPE_DRIVER_ERROR,			//A system driver error occured.
		NAUDIO_EXCEPTION_TYPE_SYSTEM_ERROR,			//A system error occured.
		NAUDIO_EXCEPTION_TYPE_THREAD_ERROR			//A thread error occured.
	};

	NAudioError(const std::string& message, NAUDIO_EXCEPTION_TYPE type = NAudioError::NAUDIO_EXCEPTION_TYPE_UNSPECIFIED) throw() :
		message_(message), type_(type)
	{
	}

	virtual ~NAudioError() throw() {
	}

	//Prints thrown error message to stderr.
	virtual void
	printMessage() const throw() {
		std::cerr << '\n' << message_ << "\n\n";
	}

	virtual const NAUDIO_EXCEPTION_TYPE&
	getType() const throw() {
		return(type_);
	}

	virtual const std::string&
	getMessage() const throw() {
		return(message_);
	}

	//Returns the thrown error message as a c-style string.
	virtual const char*
	what() const throw() {
		return(message_.c_str());
	}

protected:
	std::string message_;
	NAUDIO_EXCEPTION_TYPE type_;
};