#pragma once

#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlSnapToScale_ : public ControlConditioner_ {
		protected:
			std::vector<float> mScale;

			void
			computeOutput(const SynthesisContext_& context);

			float
			snap(float number);

		public:
			void
			setScale(std::vector<float> scale) {
				mScale = scale;
			}
		};

		inline void
		ControlSnapToScale_::computeOutput(const SynthesisContext_& context) {
			static const int NOTES_PER_OCTAVE = 12;

			if(input_.tick(context).triggered) {
				float number = input_.tick(context).value;

				int octave = (int)(number / NOTES_PER_OCTAVE);

				float baseNumber = number - (octave * NOTES_PER_OCTAVE);
				float snappedValue = snap(baseNumber) + (octave * NOTES_PER_OCTAVE);

				if(output_.value != snappedValue) {
					output_.value = snappedValue;
					output_.triggered = true;
				}
				else {
					output_.triggered = false;
				}
			}
			else {
				output_.triggered = false;
			}
		}

		inline float
		ControlSnapToScale_::snap(float number) {
			float ret = 0.0f;
			float leastDistance = -1.0f;

			for(size_t i = 0; i < mScale.size(); ++i) {
				float distance = mScale.at(i) - number;

				if(distance < 0.0f) {
					distance = -distance;
				}
				if(leastDistance == -1.0f) {
					leastDistance = distance;
				}
				if(distance <= leastDistance) {
					leastDistance = distance;
					ret = mScale.at(i);
				}
			}

			return(ret);
		}
	}

	//Given a scale, snaps the input value to the nearest scale note, in any octave.
	class ControlSnapToScale : public TemplatedControlConditioner<ControlSnapToScale, NAudio_DSP::ControlSnapToScale_> {
	public:
		ControlSnapToScale
		setScale(std::vector<float> scale) {
			gen()->setScale(scale);
			return(*this);
		}
	};
}