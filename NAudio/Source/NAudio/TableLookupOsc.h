#pragma once

#include "Generator.h"
#include "SampleTable.h"

namespace NAudio {
	namespace NAudio_DSP {
		//Registry for all static oscillator lookup table data.
		NDictionary<SampleTable>* s_oscillatorTables();

		//Many of the original STK methods are not applicable in our use case (direct phase/freq manipulation) and have been removed for optimization.
		//In the future, phase inputs may be added...
		class TableLookupOsc_ : public Generator_ {
		protected:
			SampleTable lookupTable_;

			double phase_;

			Generator frequencyGenerator_;
			NAudioFrames modFrames_;

			void
			computeSynthesisBlock(const SynthesisContext_& context);

		public:
			TableLookupOsc_();

			//Clear output and reset time pointer to zero.
			void
			reset(void);

			//Set frequency generator input.
			void
			setFrequency(Generator genArg) {
				frequencyGenerator_ = genArg;
			}

			//Set sample table for lookup. Note: must be power of 2 in length.
			void
			setLookupTable(SampleTable table);
		};

		inline void
		TableLookupOsc_::computeSynthesisBlock(const SynthesisContext_& context) {
			//Update the frequency data.
			frequencyGenerator_.tick(modFrames_, context);

			unsigned long tableSize = (unsigned long)(lookupTable_.size() - 1);
			const float rateConstant = (float)tableSize / NAudio::SampleRate();

			float* samples = &outputFrames_[0];
			float* rateBuffer = &modFrames_[0];
			float* tableData = lookupTable_.dataPointer();

			FastPhasor sd;

			//Pre-multiply rate constant for speed.
			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				*rateBuffer++ *= rateConstant;
			}

			rateBuffer = &modFrames_[0];

			sd.d = BIT32DECPT;

			int offs;
			int msbi = sd.i[1];

			double frac;
			double ps = phase_ + BIT32DECPT;

			float* tAddr;
			float f1;
			float f2;

			for(unsigned int i = 0; i < kSynthesisBlockSize; ++i) {
				sd.d = ps;
				ps += *rateBuffer++;
				offs = sd.i[1] & (tableSize - 1);
				tAddr = tableData + offs;
				sd.i[1] = msbi;
				frac = sd.d - BIT32DECPT;
				f1 = tAddr[0];
				f2 = tAddr[1];

				*samples++ = f1 + (float)frac * (f2 - f1);
			}

			sd.d = BIT32DECPT * tableSize;
			msbi = sd.i[1];
			sd.d = ps + (BIT32DECPT * tableSize - BIT32DECPT);
			sd.i[1] = msbi;
			phase_ = sd.d - BIT32DECPT * tableSize;
		}
	}

	class TableLookupOsc : public TemplatedGenerator<NAudio_DSP::TableLookupOsc_> {
	public:
		TableLookupOsc&
		setLookupTable(SampleTable lookupTable);

		NAUDIO_MAKE_GEN_SETTERS(TableLookupOsc, freq, setFrequency);
	};
}