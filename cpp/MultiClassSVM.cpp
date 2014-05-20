
#include <ctime>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "../h/MultiClassSVM.h"

void MultiClassSVM::train(const MuscleMotion& motion, const std::vector<math::Vector>& data) {

}

MuscleMotion MultiClassSVM::classify(const std::vector<math::Vector>& values) {
	clock_t t = clock();
	//TODO: implement, this is just dummy code
	MuscleMotion motion = MuscleMotion::UNKNOWN;
	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "classification took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return motion;
}