#include "TableLookupOsc.h"

namespace NAudio {
	namespace NAudio_DSP {
		NDictionary<SampleTable>* s_oscillatorTables() {
			static NDictionary<SampleTable> * s_oscillatorTables = new NDictionary<SampleTable>;
			return(s_oscillatorTables);
		}

		TableLookupOsc_::TableLookupOsc_() :
			phase_(0.0)
		{
			modFrames_.Resize(kSynthesisBlockSize, 1u);
			lookupTable_ = SampleTable(kSynthesisBlockSize, 1u);
		}

		void
		TableLookupOsc_::reset() {
			phase_ = 0.0f;
		}

		void
		TableLookupOsc_::setLookupTable(SampleTable table) {
			if(table.channels() != 1u) {
				LOG(NLOG_ERROR, "TableLookupOsc expects lookup table with 1 channel only.");
				return;
			}

			int nearestPo2;

			if(!IsPowerOf2((int)(table.size() - 1), &nearestPo2)) {
				LOG(NLOG_WARN, "TableLookUpOsc lookup tables must have a (power of two + 1) number of samples (example 2049 or 4097). Resizing to nearest power of two + 1.");

				table.resample(nearestPo2, 1u);
				table.resize(nearestPo2 + 1, 1u);
				table.dataPointer()[nearestPo2] = table.dataPointer()[0];
			}

			lookupTable_ = table;
		}
	}

	TableLookupOsc&
	TableLookupOsc::setLookupTable(SampleTable lookupTable) {
		gen()->setLookupTable(lookupTable);
		return(*this);
	}
}