#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "../h/SVMClassifier.h"

using namespace motion_classifier;

SVMClassifier::SVMClassifier(EMGProvider* emgProvider, MultiClassSVM *svm, Properties* configuration) {
	SVMClassifier::emgProvider = emgProvider;
	SVMClassifier::svm = svm;
	Interval::setMeanFunction(&math::getRMSMean);
	prop = configuration;
	variogram = new Variogram(prop->getInt("variogram.nrBins"));
	logger->info("Classifier created");
}

SVMClassifier::~SVMClassifier() {
	send(Signal::SHUTDOWN);
	//logger->info(printStatistics());
	logger->info("Classifier destroyed");
}

Motion::Muscle SVMClassifier::classify(Interval *interval) {
	logger->trace("calculating mean sample");
	Sample *mean = interval->getMeanSample();

	logger->trace("calculating Variogram");
	std::vector<math::Vector> *values = variogram->calculate(mean);

	logger->trace("classifying values");
	Motion::Muscle motion = svm->classify(values);

	//plots the calculated values
	//plot(mean, values);

	return motion;
}

void SVMClassifier::plot(Sample* sample, std::vector<math::Vector>& values) {
	if (prop->getBool("plot.mean")) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount) + "-mean.txt");
		sampleStream << *sample;
		sampleStream.close();
	}
	if (prop->getBool("plot.variogramGraph")) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount) + "-graph.txt");
		for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); ++it)
			sampleStream << it->getGroup() << "\t" << it->getLength(2) << "\t" << it->get(math::Dimension::Z) << std::endl;
		sampleStream.close();
	}
	if (prop->getBool("plot.variogramSurface")) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount) + "-surface.txt");
		for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); ++it)
			sampleStream << it->get(math::Dimension::X) << "\t" << it->get(math::Dimension::Y) << "\t" << it->get(math::Dimension::Z) << std::endl;
		sampleStream.close();
	}
}

void SVMClassifier::train(std::map<Motion::Muscle, std::vector<Interval*>*>* values) {
	for (const auto &pair : *values) {
		auto *res = new std::vector<math::Vector>;
		for (const auto &interval : *pair.second) {
			Sample *mean = interval->getMeanSample();
			std::vector<math::Vector> *vec = variogram->calculate(mean);
			res->insert(res->end(), vec->begin(), vec->end());

			delete vec;
		}
		svm->train(pair.first, res);
	}
	svm->calculateSVMs();
}