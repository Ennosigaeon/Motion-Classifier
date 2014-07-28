#ifndef KERNEL_H
#define KERNEL_H

#include "Vector.h"

namespace motion_classifier {
	namespace math {

		class Kernel {
		private:
			KernelType type;

		public:
			Kernel(KernelType type);
			double calculate(const double value) const;
		};
	}
}

#endif