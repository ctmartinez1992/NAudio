#pragma once

#include "Effect.h"
#include "FilterUtils.h"
#include "Generator.h"
#include "ControlGenerator.h"

//IIR filter modules.
//NOTE: Use of high Q values may cause clipping. It's recommended that you add a limiter or greatly reduce the gain after a filter stage.
namespace NAudio {
	namespace NAudio_DSP {
		//Basic filter Effect_ subclass with inputs for cutoff and Q.
		class Filter_ : public Effect_ {
		protected:
			NAudioFrames workspace_;

			Generator cutoff_;
			Generator Q_;

			ControlGenerator bypass_;

			bool bNormalizeGain_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

			//Subclasses override to compute new coefficients and apply filter
			virtual void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) = 0;

		public:
			Filter_();

			//Overridden so output channel layout follows input channel layout.
			virtual void
			setInput(Generator input);

			void
			setNormalizesGain(bool norm) {
				bNormalizeGain_ = norm;
			}

			void
			setCutoff(Generator cutoff) {
				cutoff_ = cutoff;
			}

			void
			setQ(Generator Q) {
				Q_ = Q;
			}

			void
			setBypass(ControlGenerator bypass) {
				bypass_ = bypass;
			}
		};

		inline void
		Filter_::computeSynthesisBlock(const SynthesisContext_& context) {
			float cCutoff;
			float cQ;

			//Get cutoff and Q inputs. For now only using first frame of output. Setting coefficients each frame is very inefficient.
			//Updating cutoff every 64-samples is typically fast enough to avoid audible artifacts when sweeping filters.
			cutoff_.tick(workspace_, context);
			cCutoff = Clamp(workspace_(0, 0u), 20.0f, SampleRate() / 2.0f);

			Q_.tick(workspace_, context);
			cQ = Max(workspace_(0, 0u), 0.7071f);

			applyFilter(cCutoff, cQ, context);
		}

		//LPF 6. One-pole lowpass filter. Q is undefined for this filter.
		class LPF6_ : public Filter_ {
		private:
			float lastOut_[2];

		protected:
			inline void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) {
				float* inptr = &dryFrames_[0];
				float* outptr = &outputFrames_[0];

				float coef = cutoffToOnePoleCoef(cutoff);
				float norm = bNormalizeGain_ ? 1.0f - coef : 1.0f;

				unsigned int nChannels = dryFrames_.Channels();

				for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
					for(unsigned int c = 0; c < nChannels; ++c) {
						lastOut_[c] = (norm * (*inptr++)) + (coef * lastOut_[c]);
						*outptr++ = lastOut_[c];
					}
				}
			}

		public:
			LPF6_() {
				lastOut_[0] = 0;
				lastOut_[1] = 0;
			}
		};

		//HPF 6. One-pole highpass filter. Q is undefined for this filter.
		class HPF6_ : public Filter_ {
		private:
			float lastOut_[2];

		protected:
			inline void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) {
				float* inptr = &dryFrames_[0];
				float* outptr = &outputFrames_[0];

				float coef = 1.0f - cutoffToOnePoleCoef(cutoff);
				float norm = bNormalizeGain_ ? 1.0f - coef : 1.0f;

				unsigned int nChannels = dryFrames_.Channels();

				for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
					for(unsigned int c = 0; c < nChannels; ++c) {
						lastOut_[c] = (norm * (*inptr++)) - (coef * lastOut_[c]);
						*outptr++ = lastOut_[c];
					}
				}
			}

		public:
			HPF6_() {
				lastOut_[0] = 0;
				lastOut_[1] = 0;
			}
		};

		//LPF 12. Butterworth 2-pole LPF.
		class LPF12_ : public Filter_ {
		private:
			Biquad biquad_;

		protected:
			inline void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) {
				//Set coefficients.
				float newCoef[5];

				bltCoef(0.0f, 0.0f, bNormalizeGain_ ? 1.0f / Q : 1.0f, 1.0f / Q, 1.0f, cutoff, newCoef);
				biquad_.setCoefficients(newCoef);

				//Compute.
				biquad_.filter(dryFrames_, outputFrames_);
			}

		public:
			void
			setIsStereoInput(bool isStereoInput) {
				Filter_::setIsStereoInput(isStereoInput);
				biquad_.setIsStereo(isStereoInput);
			}
		};

		//LPF 24. Butterworth 4-pole LPF.
		class LPF24_ : public Filter_ {
		private:
			Biquad biquads_[2];

		protected:
			inline void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) {
				//Set coefficients.
				float newCoef[5];

				//Stage 1.
				bltCoef(0.0f, 0.0f, bNormalizeGain_ ? 1.0f / Q : 1.0f, 0.5412f / Q, 1.0f, cutoff, newCoef);
				biquads_[0].setCoefficients(newCoef);

				//Stage 2.
				bltCoef(0.0f, 0.0f, bNormalizeGain_ ? 1.0f / Q : 1.0f, 1.3066f / Q, 1.0f, cutoff, newCoef);
				biquads_[1].setCoefficients(newCoef);

				//Compute.
				biquads_[0].filter(dryFrames_, outputFrames_);
				biquads_[1].filter(outputFrames_, outputFrames_);
			}

		public:
			void
			setIsStereoInput(bool isStereoInput) {
				Filter_::setIsStereoInput(isStereoInput);
				biquads_[0].setIsStereo(isStereoInput);
				biquads_[1].setIsStereo(isStereoInput);
			}
		};

		//HPF 12. Butterworth 2-pole HPF.
		class HPF12_ : public Filter_ {
		private:
			Biquad biquad_;

		protected:
			inline void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) {
				//Set coefficients.
				float newCoef[5];

				bltCoef(bNormalizeGain_ ? 1.0f / Q : 1.0f, 0.0f, 0.0f, 1.0f / Q, 1.0f, cutoff, newCoef);
				biquad_.setCoefficients(newCoef);

				//Compute.
				biquad_.filter(dryFrames_, outputFrames_);
			}

		public:
			void
			setIsStereoInput(bool isStereoInput) {
				Filter_::setIsStereoInput(isStereoInput);
				biquad_.setIsStereo(isStereoInput);
			}
		};

		//HPF 24. Butterworth 4-pole HPF.
		class HPF24_ : public Filter_ {
		private:
			Biquad biquads_[2];

		protected:
			inline void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) {
				//Set coefficients.
				float newCoef[5];

				//Stage 1.
				bltCoef(bNormalizeGain_ ? 1.0f / Q : 1.0f, 0.0f, 0.0f, 0.5412f / Q, 1.0f, cutoff, newCoef);
				biquads_[0].setCoefficients(newCoef);

				//Stage 2.
				bltCoef(bNormalizeGain_ ? 1.0f / Q : 1.0f, 0.0f, 0.0f, 1.3066f / Q, 1.0f, cutoff, newCoef);
				biquads_[1].setCoefficients(newCoef);

				//Compute.
				biquads_[0].filter(dryFrames_, outputFrames_);
				biquads_[1].filter(outputFrames_, outputFrames_);
			}

		public:
			void
			setIsStereoInput(bool isStereoInput) {
				Filter_::setIsStereoInput(isStereoInput);
				biquads_[0].setIsStereo(isStereoInput);
				biquads_[1].setIsStereo(isStereoInput);
			}
		};

		//BPF 12. Butterworth 2-pole BPF, constant 0dB peak
		class BPF12_ : public Filter_ {
		private:
			Biquad biquad_;

		protected:
			inline void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) {
				//Set coefficients.
				float newCoef[5];

				bltCoef(0.0f, bNormalizeGain_ ? 1.0f / Q : 1.0f, 0.0f, 1.0f / Q, 1.0f, cutoff, newCoef);
				biquad_.setCoefficients(newCoef);

				//Compute.
				biquad_.filter(dryFrames_, outputFrames_);
			}

		public:
			virtual void
			setIsStereoInput(bool isStereoInput) {
				Filter_::setIsStereoInput(isStereoInput);
				biquad_.setIsStereo(isStereoInput);
			}
		};

		//BPF 24. Butterworth 4-pole BPF.
		class BPF24_ : public Filter_ {
		private:
			Biquad biquads_[2];

		protected:
			inline void
			applyFilter(float cutoff, float Q, const SynthesisContext_& context) {
				//Set coefficients.
				float newCoef[5];

				//Stage 1.
				bltCoef(0.0f, bNormalizeGain_ ? 1.0f / Q : 1.0f, 0.0f, 0.5412f / Q, 1.0f, cutoff, newCoef);
				biquads_[0].setCoefficients(newCoef);

				//Stage 2.
				bltCoef(0.0f, bNormalizeGain_ ? 1.0f / Q : 1.0f, 0.0f, 1.3066f / Q, 1.0f, cutoff, newCoef);
				biquads_[1].setCoefficients(newCoef);

				//Compute.
				biquads_[0].filter(dryFrames_, outputFrames_);
				biquads_[1].filter(outputFrames_, outputFrames_);
			}

		public:
			void
			setIsStereoInput(bool isStereoInput) {
				Filter_::setIsStereoInput(isStereoInput);
				biquads_[0].setIsStereo(isStereoInput);
				biquads_[1].setIsStereo(isStereoInput);
			}
		};
	}

	//Smart Pointers.
	template<class FilterType, class FilterType_>
	class TemplatedFilter : public TemplatedEffect<FilterType, FilterType_> {
	public:
		NAUDIO_MAKE_GEN_SETTERS(FilterType, cutoff, setCutoff);
		NAUDIO_MAKE_GEN_SETTERS(FilterType, Q, setQ);

		FilterType&
		normalizesGain(bool norm) {
			this->gen()->setNormalizesGain(norm);
			return(static_cast<FilterType&>(*this));
		}
	};

	//LPF 6.
	class LPF6 : public TemplatedFilter<LPF6, NAudio_DSP::LPF6_> {
	};

	//HPF 6.
	class HPF6 : public TemplatedFilter<HPF6, NAudio_DSP::HPF6_> {
	};

	//LPF 12.
	class LPF12 : public TemplatedFilter<LPF12, NAudio_DSP::LPF12_> {
	};

	//LPF 24.
	class LPF24 : public TemplatedFilter<LPF24, NAudio_DSP::LPF24_> {
	};

	//HPF 12.
	class HPF12 : public TemplatedFilter<HPF12, NAudio_DSP::HPF12_> {
	};

	//HPF 24.
	class HPF24 : public TemplatedFilter<HPF24, NAudio_DSP::HPF24_> {
	};

	//BPF 12.
	class BPF12 : public TemplatedFilter<BPF12, NAudio_DSP::BPF12_> {
	};

	//BPF 24.
	class BPF24 : public TemplatedFilter<BPF24, NAudio_DSP::BPF24_> {
	};
}