
#include <array>
#include "../h/Space.h"

using namespace motion_classifier;

math::Norm math::Space::norm = math::Norm::L2;

math::Space::Space(double minX, double maxX, double minY, double maxY, double minZ, double maxZ) {
	min[math::Dimension::X] = minX;
	min[math::Dimension::Y] = minY;
	min[math::Dimension::Z] = minZ;
	max[math::Dimension::X] = maxX;
	max[math::Dimension::Y] = maxY;
	max[math::Dimension::Z] = maxZ;
}

double math::Space::getMin(math::Dimension dimension) const {
	return min[dimension];
}

double math::Space::getMax(math::Dimension dimension) const {
	return max[dimension];
}
