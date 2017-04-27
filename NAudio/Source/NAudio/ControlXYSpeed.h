#pragma once

#include "ControlGenerator.h"
#include <list>

namespace NAudio {
	namespace NAudio_DSP {
		class ControlXYSpeed_ : public ControlGenerator_ {
		protected:
			std::list<float> vals;

			ControlGenerator x;
			ControlGenerator y;

			float lastXVal;
			float lastYVal;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlXYSpeed_();

			void
			setX(ControlGenerator x);

			void
			setY(ControlGenerator y);
		};

		inline void
		ControlXYSpeed_::computeOutput(const SynthesisContext_& context) {
			const int numToAverage = 1;

			ControlGeneratorOutput xOut = x.tick(context);
			ControlGeneratorOutput yOut = y.tick(context);

			if(xOut.triggered || yOut.triggered) {
				output_.triggered = true;

				float dX = xOut.value - lastXVal;
				float dY = yOut.value - lastYVal;

				float speed = sqrt(dX * dX + dY * dY);

				vals.push_back(speed);

				if(vals.size() > numToAverage) {
					vals.pop_front();
				}

				float total = 0.0f;

				for(std::list<float>::iterator it = vals.begin(); it != vals.end(); ++it) {
					total += *it;
				}

				output_.value = total / vals.size();

				lastXVal = xOut.value;
				lastYVal = yOut.value;
			}
			else {
				output_.triggered = false;
			}
		}
	}

	//Given a point described by an x and y coordinate, calculate the speed that the point is moving. This is designed for mouse/finger speed, but could potentially have other uses.
	class ControlXYSpeed : public TemplatedControlGenerator<NAudio_DSP::ControlXYSpeed_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlXYSpeed, x, setX);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlXYSpeed, y, setY);
	};
}