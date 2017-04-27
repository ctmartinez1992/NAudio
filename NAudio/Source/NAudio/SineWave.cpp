#include "SineWave.h"

namespace NAudio {
	SineWave::SineWave() {
		static std::string const NAUDIO_SIN_TABLE = "_NAUDIO_SIN_TABLE_";

		//As soon as the first SineWave is allocated, persistent SampleTable is created. Will stay in memory for program lifetime.
		if(!NAudio_DSP::s_oscillatorTables()->ContainsObjectNamed(NAUDIO_SIN_TABLE)) {
			const unsigned int tableSize = 4096;

			SampleTable sineTable = SampleTable(tableSize + 1, 1);

			float norm = 1.0f / tableSize;
			float* data = sineTable.dataPointer();

			for(unsigned long i = 0; i < tableSize + 1; ++i) {
				*data++ = sinf(TWO_PI * i * norm);
			}

			NAudio_DSP::s_oscillatorTables()->InsertObject(NAUDIO_SIN_TABLE, sineTable);

			this->gen()->setLookupTable(sineTable);
		}
		else {
			this->gen()->setLookupTable(NAudio_DSP::s_oscillatorTables()->ObjectNamed(NAUDIO_SIN_TABLE));
		}
	}
}