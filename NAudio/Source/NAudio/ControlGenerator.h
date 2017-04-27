#pragma once

#include "NAudioCore.h"

namespace NAudio {
	struct ControlGeneratorOutput {
		float value;
		bool triggered;
		
		ControlGeneratorOutput() :
			value(0), triggered(false)
		{
		}
	};
	
	namespace NAudio_DSP {
		class ControlGenerator_ {
		protected:
			ControlGeneratorOutput output_;
			unsigned long lastFrameIndex_;

			//Override this function to implement a new ControlGenerator. Subclasses should use this function to put new data into output_.
			virtual void
				computeOutput(const SynthesisContext_& context) {
			}

		public:
			ControlGenerator_();
			virtual ~ControlGenerator_();
            
			//Mutex for swapping inputs.
			void lockMutex();
			void unlockMutex();
      
			//Only override tick if you need custom reuse behavior. Pass in a pointer to a float to return a value. Some generators may not care about value.
			virtual ControlGeneratorOutput
			tick(const SynthesisContext_& context);
      
			//Used for initializing other generators (see smoothed() method for example).
			virtual ControlGeneratorOutput
			initialOutput();
		};
    
		inline ControlGeneratorOutput
		ControlGenerator_::tick(const SynthesisContext_& context) {
			if(context.forceNewOutput || lastFrameIndex_ != context.elapsedFrames) {
				lastFrameIndex_ = context.elapsedFrames;
				computeOutput(context);
			}
			
			if(output_.value != output_.value) {
				LOG(NLOG_ERROR, "NaN detected.");
			}
			
			return(output_);
		}
	}
	
	class RampedValue;
	
	class ControlGenerator : public NSmartPointer<NAudio_DSP::ControlGenerator_> {
	public:
		ControlGenerator(NAudio_DSP::ControlGenerator_* cGen = new NAudio_DSP::ControlGenerator_) :
			NSmartPointer<NAudio_DSP::ControlGenerator_>(cGen)
		{
		}
		
		inline ControlGeneratorOutput
		tick(const NAudio_DSP::SynthesisContext_& context) {
			return(obj->tick(context));
		}
		
		//Shortcut for creating ramped value.
		RampedValue smoothed(float length = 0.05f);
	};
	
	template<class GenType>
	class TemplatedControlGenerator : public ControlGenerator {
	protected:
		GenType*
		gen() {
			return(static_cast<GenType*>(obj));
		}
    
	public:
		TemplatedControlGenerator() :
			ControlGenerator(new GenType)
		{
		}
	};
}

#include "ControlValue.h"

#define NAUDIO_MAKE_CTRL_GEN_SETTERS(generatorClassName, methodNameInGenerator, methodNameInGenerator_)	\
																										\
generatorClassName& methodNameInGenerator(float arg) {													\
	return(methodNameInGenerator(ControlValue(arg)));													\
}																										\
generatorClassName& methodNameInGenerator(ControlGenerator arg) {										\
	this->gen()->methodNameInGenerator_(arg);															\
	return(static_cast<generatorClassName&>(*this));													\
}