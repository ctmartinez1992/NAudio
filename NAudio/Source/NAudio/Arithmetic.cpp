#include "Arithmetic.h"

namespace NAudio {
	namespace NAudio_DSP {
		Adder_::Adder_() {
			workSpace_.Resize(kSynthesisBlockSize, 1, 0);
		}

		void
		Adder_::input(Generator generator) {
			inputs_.push_back(generator);

			if(generator.isStereoOutput() && !this->isStereoOutput()) {
				setIsStereoOutput(true);
			}
		}

		void
		Adder_::setIsStereoOutput(bool stereo) {
			Generator_::setIsStereoOutput(stereo);
			workSpace_.Resize(kSynthesisBlockSize, stereo ? 2u : 1u, 0.0f);
		}

		Subtractor_::Subtractor_() {
			workSpace_.Resize(kSynthesisBlockSize, 1u, 0.0f);
		}

		void
		Subtractor_::setLeft(Generator arg) {
			if(arg.isStereoOutput() && !isStereoOutput()) {
				setIsStereoOutput(true);
			}

			left_ = arg;
		}

		void
		Subtractor_::setRight(Generator arg) {
			if(arg.isStereoOutput() && !isStereoOutput()) {
				setIsStereoOutput(true);
			}

			right_ = arg;
		}

		void
		Subtractor_::setIsStereoOutput(bool stereo) {
			Generator_::setIsStereoOutput(stereo);
			workSpace_.Resize(kSynthesisBlockSize, stereo ? 2u : 1u, 0.0f);
		}

		Multiplier_::Multiplier_() {
			workSpace_.Resize(kSynthesisBlockSize, 1u, 0.0f);
		}

		void
		Multiplier_::input(Generator generator) {
			inputs_.push_back(generator);

			if(generator.isStereoOutput() && !isStereoOutput()) {
				setIsStereoOutput(true);
			}
		}

		void
		Multiplier_::setIsStereoOutput(bool stereo) {
			Generator_::setIsStereoOutput(stereo);

			workSpace_.Resize(kSynthesisBlockSize, stereo ? 2u : 1u, 0.0f);
		}

		Divider_::Divider_() {
			workSpace_.Resize(kSynthesisBlockSize, 1u, 0.0f);
		}

		void
		Divider_::setLeft(Generator arg) {
			if(arg.isStereoOutput() && !isStereoOutput()) {
				setIsStereoOutput(true);
			}

			left_ = arg;
		}

		void
		Divider_::setRight(Generator arg) {
			if(arg.isStereoOutput() && !isStereoOutput()) {
				setIsStereoOutput(true);
			}

			right_ = arg;
		}

		void Divider_::setIsStereoOutput(bool stereo) {
			Generator_::setIsStereoOutput(stereo);
			workSpace_.Resize(kSynthesisBlockSize, stereo ? 2u : 1u, 0.0f);
		}
	}
}