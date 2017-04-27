#pragma once

#include "SampleTable.h"

namespace NAudio {
	SampleTable loadAudioFile(std::string path, int numChannels = 2);
}