#pragma once

#include "ControlConditioner.h"

namespace NAudio {
	namespace NAudio_DSP {
		class ControlMetroDivider_ : public ControlConditioner_ {
		protected:
			ControlGenerator divisonsGen_;
			ControlGenerator offsetGen_;

			unsigned int offset_;
			unsigned int tickCounter_;

			void
			computeOutput(const SynthesisContext_& context);

		public:
			ControlMetroDivider_();

			void
			setDivisionsGen(ControlGenerator gen) {
				divisonsGen_ = gen;
			}
			void
			setOffsetGen(ControlGenerator gen) {
				offsetGen_ = gen;
			}
		};

		inline
		void ControlMetroDivider_::computeOutput(const SynthesisContext_& context) {
			offset_ = (unsigned int)(offsetGen_.tick(context).value);

			unsigned int divisions = (unsigned int)Max(1.0f, divisonsGen_.tick(context).value);

			output_.triggered = false;

			if(input_.tick(context).triggered) {
				unsigned int modcount = (tickCounter_++ + offset_) % divisions;

				if(modcount == 0u) {
					output_.triggered = true;
				}

				if(tickCounter_ >= divisions) {
					tickCounter_ = 0u;
				}
			}
		}
	}

	class ControlMetroDivider : public TemplatedControlConditioner<ControlMetroDivider, NAudio_DSP::ControlMetroDivider_> {
	public:
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlMetroDivider, divisions, setDivisionsGen);
		NAUDIO_MAKE_CTRL_GEN_SETTERS(ControlMetroDivider, offset, setOffsetGen);
	};
}