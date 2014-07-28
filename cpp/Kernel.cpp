#include "../h/Kernel.h"

using namespace motion_classifier::math;

Kernel::Kernel(KernelType type) {
	Kernel::type = type;
}

double Kernel::calculate(const double value) const{
	switch (type) {
	case KernelType::UNIFORM:
		if (value <= 1)
			return 1;
		return 0;
	case KernelType::GAUSSIAN:
		return exp(-value / 2 );
	case KernelType::EPANECHNIKOV:
	default:
		if (value <= 1)
			return 1 - value;
		return 0;
	}
}
