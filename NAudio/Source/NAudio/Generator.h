#pragma once

#include "NAudioFrames.h"

namespace NAudio {
	namespace NAudio_DSP {
		class Generator_ {
		public:
			Generator_();
			virtual ~Generator_();

			virtual void
			tick(NAudioFrames& frames, const SynthesisContext_& context);

			bool
			isStereoOutput() {
				return(isStereoOutput_);
			}

			//Set stereo/mono - changes number of channels in outputFrames_. Subclasses should call in constructor to determine channel output.
			virtual void
			setIsStereoOutput(bool stereo);

		protected:
			NAudioFrames outputFrames_;

			unsigned long lastFrameIndex_;

			bool isStereoOutput_;

			//Override point for defining generator behavior. Subclasses should implement to fill frames with new data.
			virtual void
			computeSynthesisBlock(const SynthesisContext_&context) {
			}
		};

		inline void
		Generator_::tick(NAudioFrames& frames, const SynthesisContext_& context) {
			//Check context to see if we need new frames.
			if(context.forceNewOutput || lastFrameIndex_ != context.elapsedFrames) {
				computeSynthesisBlock(context);
				lastFrameIndex_ = context.elapsedFrames;
			}

			//Copy synthesis block to frames passed in.
			frames.Copy(outputFrames_);
		}
	}

	class Generator : public NSmartPointer<NAudio_DSP::Generator_> {
	public:
		Generator(NAudio_DSP::Generator_* gen = new NAudio_DSP::Generator_) :
			NSmartPointer<NAudio_DSP::Generator_>(gen)
		{
		}

		inline bool
		isStereoOutput() {
			return(obj->isStereoOutput());
		}

		virtual void
		tick(NAudioFrames& frames, const NAudio_DSP::SynthesisContext_& context) {
			obj->tick(frames, context);
		}
	};

	template<class GenType>
	class TemplatedGenerator : public Generator {
	protected:
		GenType* gen() {
			return(static_cast<GenType*>(obj));
		}

	public:
		TemplatedGenerator() :
			Generator(new GenType)
		{
		}
	};
}

//Each generator should have three flavors of setter: One that accepts a float, one that accepts a ControlGenerator, and one that accepts a Generator. This macro will automatically build those three setters.
#include "FixedValue.h"

#define NAUDIO_MAKE_GEN_SETTERS(generatorClassName, methodNameInGenerator, methodNameInGenerator_)		\
	generatorClassName&																					\
	methodNameInGenerator(Generator arg) {																\
		this->gen()->methodNameInGenerator_(arg);														\
		return(static_cast<generatorClassName&>(*this));												\
	}																									\
																										\
	generatorClassName&																					\
	methodNameInGenerator(float arg) {																	\
		return(methodNameInGenerator(FixedValue(arg)));													\
	}																									\
																										\
	generatorClassName&																					\
	methodNameInGenerator(ControlGenerator arg)	{														\
		return(methodNameInGenerator( FixedValue().setValue(arg)));										\
	}