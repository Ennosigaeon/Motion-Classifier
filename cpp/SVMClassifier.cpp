#include <ctime>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "../h/Logger.h"
#include "../h/SVMClassifier.h"
#include "../h/Utilities.h"

using namespace motion_classifier;

SVMClassifier::SVMClassifier(EMGProvider* emgProvider, MultiClassSVM *svm, Properties* configuration) {
	SVMClassifier::emgProvider = emgProvider;
	SVMClassifier::svm = svm;
	Interval::setMeanFunction(&math::getRMSMean);
	prop = configuration;
	variogram = &Variogram(prop->getInt("variogram.nrBins"));
	Logger::getInstance()->info("Classifier created");
}

SVMClassifier::~SVMClassifier() {
	send(Signal::SHUTDOWN);
	Logger::getInstance()->info(printStatistics());
	Logger::getInstance()->info("Classifier destroyed");
}

void SVMClassifier::run() {
	Logger *logger = Logger::getInstance();
	while (true) {
		if (status == Status::RUNNING) {
			logger->debug("waiting for new Interval");
			Interval *interval = emgProvider->getInterval();
			if (interval == NULL)
				continue;

			clock_t t = clock();
			logger->debug("calculating mean sample");
			Sample *mean = interval->getMeanSample();
			if (mean == NULL)
				continue;

			logger->debug("calculating Variogram");
			std::vector<math::Vector> values = variogram->calculate(mean);

			logger->debug("classifying values");
			Motion::Muscle motion = svm->classify(values);

			//plots the calculated values
			plot(mean, values);

			//overrides the last stored value
			lastMuscleMotion.push(&motion);
			t = clock() - t;
			double tmp = ((double)t) / CLOCKS_PER_SEC * 1000;
			logger->info("classified new Interval in " + boost::lexical_cast<std::string>(tmp) + " ms as " + motion_classifier::printMotion(motion));

			time += tmp;
			++intervalCount;
			delete interval;
		}
		if (status == Status::WAITING) {
			logger->debug("Classifier stops processing Intervals");
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait(lk);
		}
		if (status == Status::FINISHED) {
			logger->info("shuting down SVMClassifier worker");
			return;
		}
	}
}

void SVMClassifier::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (status == Status::NEW)  {
			//start EMGProvider
			emgProvider->send(Signal::START);

			//start worker thread
			status = Status::RUNNING;
			worker = std::thread(&SVMClassifier::run, this);
		}
		else {
			status = Status::RUNNING;
			std::unique_lock<std::mutex> mlock(mutex);
			mlock.unlock();
			condition.notify_one();
		}
	}
	if (signal == Signal::STOP) {
		status = Status::WAITING;
		emgProvider->send(Signal::STOP);
	}
	if (signal == Signal::SHUTDOWN) {
		status = Status::FINISHED;
		//release waiting thread
		std::unique_lock<std::mutex> mlock(mutex);
		mlock.unlock();
		condition.notify_one();

		//wait for worker to stop
		if (worker.joinable())
			worker.join();

		//stop the EMGProvider
		emgProvider->send(Signal::SHUTDOWN);
	}
}

void SVMClassifier::plot(Sample* sample, std::vector<math::Vector>& values) {
	if (prop->getBool("plot.mean")) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount)+"-mean.txt");
		sampleStream << *sample;
		sampleStream.close();
	}
	if (prop->getBool("plot.variogramGraph")) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount) + "-graph.txt");
		for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); ++it)
			sampleStream << it->getGroup() << "\t" << it->getLength(2) << "\t" << it->getZ() << std::endl;
		sampleStream.close();
	}
	if (prop->getBool("plot.variogramSurface")) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount) + "-surface.txt");
		for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); ++it)
			sampleStream << it->getX() << "\t" << it->getY() << "\t" << it->getZ() << std::endl;
		sampleStream.close();
	}
}

void SVMClassifier::train(const std::map<Motion::Muscle, std::vector<Interval*>>& values) {
	for (std::map<Motion::Muscle, std::vector<Interval*>>::const_iterator it = values.begin(); it != values.end(); ++it) {
		std::vector<math::Vector> res;
		for (std::vector<Interval*>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			Sample *mean = (*it2)->getMeanSample();
			std::vector<math::Vector> vec = variogram->calculate(mean);
			res.insert(res.end(), vec.begin(), vec.end());
			delete mean;
		}
		svm->train(it->first, res);
	}
	svm->calculateSVMs();
}