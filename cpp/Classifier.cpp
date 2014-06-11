#include <ctime>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/Classifier.h"
#include "../h/Utilities.h"

Classifier::Classifier(EMGProvider* emgProvider, MultiClassSVM *svm) {
	config = AppConfig::getInstance();
	Classifier::emgProvider = emgProvider;
	Classifier::svm = svm;
	BOOST_LOG_TRIVIAL(info) << "Classifier created";
}

Classifier::~Classifier() {
	send(Signal::SHUTDOWN);
	BOOST_LOG_TRIVIAL(info) << "Classified " << intervalCount << " Samples in avg. " << time / intervalCount << " ms";
	BOOST_LOG_TRIVIAL(info) << "Classifier destroyed";
}

Motion::Muscle Classifier::getMuscleMotion() {
	Motion::Muscle *motion = lastMuscleMotion.pop();
	if (motion == NULL)
		return Motion::Muscle::UNKNOWN;
	else
		return *motion;
}

void Classifier::run() {
	while (true) {
		if (status == Status::RUNNING) {
			BOOST_LOG_TRIVIAL(debug) << "waiting for new Interval";
			Interval *interval = emgProvider->getInterval();
			if (interval == NULL)
				continue;

			clock_t t = clock();
			BOOST_LOG_TRIVIAL(debug) << "calculating RMS sample";
			Sample *rms = interval->getRMSSample();
			if (rms == NULL)
				continue;

			BOOST_LOG_TRIVIAL(debug) << "calculating Variogram";
			std::vector<math::Vector> values = variogram.calculate(rms);

			BOOST_LOG_TRIVIAL(debug) << "classifying values";
			Motion::Muscle motion = svm->classify(values);

			//plots the calculated values
			plot(rms, values);

			//overrides the last stored value
			lastMuscleMotion.push(&motion);
			t = clock() - t;
			double tmp = ((double)t) / CLOCKS_PER_SEC * 1000;
			BOOST_LOG_TRIVIAL(info) << "classified new Interval in " << tmp << " ms as " << printMotion(motion);

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
			BOOST_LOG_TRIVIAL(info) << "shuting down Classifier worker";
			return;
		}
	}
}

void Classifier::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (status == Status::NEW)  {
			//start EMGProvider
			emgProvider->send(Signal::START);

			//start worker thread
			status = Status::RUNNING;
			worker = std::thread(&Classifier::run, this);
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

//TODO: is this really necessary?
//TODO: no real-time plotting. Change this!
void Classifier::plot(Sample* sample, std::vector<math::Vector>& values) {
	//TODO: create folder if it does not exists
	if (config->isPlotRMS()) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount)+"-rms.txt");
		sampleStream << *sample;
		sampleStream.close();
	}
	if (config->isPlotVariogramGraph()) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount) + "-graph.txt");
		for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); ++it)
			sampleStream << it->getGroup() << "\t" << it->getLength(2) << "\t" << it->getZ() << std::endl;
		sampleStream.close();
	}
	if (config->isPlotVariogramSurface()) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(intervalCount) + "-surface.txt");
		for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); ++it)
			sampleStream << it->getX() << "\t" << it->getY() << "\t" << it->getZ() << std::endl;
		sampleStream.close();
	}
}