#pragma once

//See:
//http://www.experimentalscene.com/articles/minbleps.php
//http://www.musicdsp.org/showArchiveComment.php?ArchiveID=112

#include "Generator.h"

namespace NAudio {
	namespace NAudio_DSP {
		class BLEPOscillator_ : public Generator_ {
		protected:
			//Input generators.
			Generator freqGen_;
			NAudioFrames freqFrames_;

			//TODO: Hardsync?

			//Lookup table.
			static const float minBLEP_[];
			static const int minBLEPlength_;
			static const int minBLEPOversampling_;

			//Phase accumulator.
			float phase_;

			//Ring buffer and accumulator.
			float* ringBuf_;
			float accum_;

			int lBuffer_;		//Ring buffer length.
			int iBuffer_;		//Current index.
			int nInit_;			//Number of initialzed samples in ring buffer.

			//Add a BLEP to the ring buffer at the specified offset.
			inline void
			addBLEP(float offset, float scale) {
				float bufOffset = minBLEPOversampling_ * offset;

				float f;
				float* outptr = ringBuf_ + iBuffer_;
				float* inptr = (float*)minBLEP_ + (int)bufOffset;
				float* bufEnd = ringBuf_ + lBuffer_;

				float frac = fmodf(bufOffset, 1.0f);

				//Add.
				int i;

				for(i = 0; i < nInit_; ++i, inptr += minBLEPOversampling_, ++outptr) {
					if(outptr >= bufEnd) {
						outptr = ringBuf_;
					}

					f = Lerp(inptr[0], inptr[1], frac);
					*outptr += scale * (1.0f - f);
				}

				//Copy.
				for(; i < lBuffer_ - 1; ++i, inptr += minBLEPOversampling_, ++outptr) {
					if(outptr >= bufEnd) {
						outptr = ringBuf_;
					}

					f = Lerp(inptr[0], inptr[1], frac);
					*outptr = scale * (1.0f - f);
				}

				nInit_ = lBuffer_ - 1;
			}

		public:
			BLEPOscillator_();
			~BLEPOscillator_();

			void
			setFreqGen(Generator gen) {
				freqGen_ = gen;
			}
		};
	}
}