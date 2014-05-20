
#include <cmath>
#include <ctime>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "../h/AppConfig.h"
#include "../h/Interval.h"
#include "../h/Plotter.h"

Interval::Interval() {
	MAX_SIZE = AppConfig::getInstance()->getIntervalNrSamples();
}

bool Interval::isFull() const {
	return samples.size() == MAX_SIZE;
}

void Interval::addSample(const Sample& sample) {
	samples.push_back(sample);
}

Sample Interval::getRMSSample() {
	BOOST_LOG_TRIVIAL(trace) << "calculating RMS Sample";
	if (samples.empty())
		return Sample{ 0, 0 };

	clock_t t = clock();
	Sample start = samples.at(0);
	Sample result{ start.getNrRows(), start.getNrColumns() };
	double *value = new double[result.getNrRows() * result.getNrColumns()];
	int *count = new int[result.getNrRows() * result.getNrColumns()];
	for (int i = 0; i < result.getNrRows() * result.getNrColumns(); i++) {
		value[i] = 0;
		count[i] = 0;
	}

	for (std::vector<Sample>::iterator it = samples.begin(); it != samples.end(); it++) {
		int i = 0;
		std::vector<math::Vector> entries = it->getEntries();
		for (std::vector<math::Vector>::iterator it2 = entries.begin(); it2 != entries.end(); it2++) {
			double n = it2->getZ();
			if (n != NAN) {
				value[i] += n * n;
				count[i]++;
			}
			i++;
		}
	}
	int i = 0;
	std::vector<math::Vector> entries = start.getEntries();
	for (std::vector<math::Vector>::iterator it = entries.begin(); it != entries.end(); it++) {
		result.addEntry(math::Vector(it->getX(), it->getY(), sqrt(value[i] / count[i])));
		i++;
	}
	delete[] value;
	delete[] count;

	t = clock() - t;
	BOOST_LOG_TRIVIAL(debug) << "RMS calculation took " << ((double)t) / CLOCKS_PER_SEC * 1000 << " ms";

	return result;
}

std::vector<Sample> Interval::getSamples() const {
	return samples;
}
