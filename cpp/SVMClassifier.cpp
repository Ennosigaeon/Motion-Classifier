#include <ctime>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/SVMClassifier.h"
#include "../h/Utilities.h"

using namespace motion_classifier;

SVMClassifier::SVMClassifier(EMGProvider* emgProvider, MultiClassSVM *svm, Properties* configuration) {
	SVMClassifier::emgProvider = emgProvider;
	SVMClassifier::svm = svm;
	Interval::setMeanFunction(&math::getRMSMean);
	prop = configuration;
	variogram = &Variogram(prop->getInt("variogram.nrBins"));
	BOOST_LOG_TRIVIAL(info) << "Classifier created";
}

SVMClassifier::~SVMClassifier() {
	send(Signal::SHUTDOWN);
	BOOST_LOG_TRIVIAL(info) << printStatistics();
	BOOST_LOG_TRIVIAL(info) << "Classifier destroyed";
}

std::string SVMClassifier::printStatistics() {
	std::string s("Classified " + boost::lexical_cast<std::string>(intervalCount) + " Samples in avg. " + boost::lexical_cast<std::string>(time / intervalCount) + " ms");
	return s;
}

Motion::Muscle SVMClassifier::getMuscleMotion() {
	Motion::Muscle *motion = lastMuscleMotion.pop();
	if (motion == NULL)
		return Motion::Muscle::UNKNOWN;
	else
		return *motion;
}

void SVMClassifier::run() {
	while (true) {
		if (status == Status::RUNNING) {
			BOOST_LOG_TRIVIAL(debug) << "waiting for new Interval";
			Interval *interval = emgProvider->getInterval();
			if (interval == NULL)
				continue;

			clock_t t = clock();
			BOOST_LOG_TRIVIAL(debug) << "calculating mean sample";
			Sample *mean = interval->getMeanSample();
			if (mean == NULL)
				continue;

			BOOST_LOG_TRIVIAL(debug) << "calculating Variogram";
			std::vector<math::Vector> values = variogram->calculate(mean);

			BOOST_LOG_TRIVIAL(debug) << "classifying values";
			Motion::Muscle motion = svm->classify(values);

			//plots the calculated values
			plot(mean, values);

			//overrides the last stored value
			lastMuscleMotion.push(&motion);
			t = clock() - t;
			double tmp = ((double)t) / CLOCKS_PER_SEC * 1000;
			BOOST_LOG_TRIVIAL(info) << "classified new Interval in " << tmp << " ms as " << motion_classifier::printMotion(motion);

			time += tmp;
			++intervalCount;
			delete interval;
		}
		if (status == Status::WAITING) {
			BOOST_LOG_TRIVIAL(debug) << "Classifier stops processing Intervals";
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait(lk);
		}
		if (status == Status::FINISHED) {
			BOOST_LOG_TRIVIAL(info) << "shuting down SVMClassifier worker";
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