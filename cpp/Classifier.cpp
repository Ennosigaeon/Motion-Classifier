#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/Classifier.h"
#include "../h/Exception.h"

Classifier::Classifier(EMGProvider* emgProvider, MultiClassSVM *svm) {
	config = AppConfig::getInstance();
	Classifier::emgProvider = emgProvider;
	Classifier::svm = svm;
	BOOST_LOG_TRIVIAL(info) << "Classifier created";
}

Classifier::~Classifier() {
	send(Signal::SHUTDOWN);
	BOOST_LOG_TRIVIAL(info) << "Classified " << nr << " Samples in avg. " << time / nr << " ms";
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

			BOOST_LOG_TRIVIAL(debug) << "calculation Variogram";
			std::vector<math::Vector> values = variogram.calculate(rms);

			BOOST_LOG_TRIVIAL(debug) << "classifying values";
			Motion::Muscle motion = svm->classify(values);

			//plots the calculated values
			plot(rms, values);

			//overrides the last stored value
			lastMuscleMotion.push(&motion);
			t = clock() - t;
			long tmp = ((double)t) / CLOCKS_PER_SEC * 1000;
			BOOST_LOG_TRIVIAL(info) << "classified new Interval in " << tmp << " ms as " << motion;

			time += tmp;
			nr++;

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
	//TODO: maybe it is good to stop EMGProvider too
	if (signal == Signal::STOP)
		status = Status::WAITING;
	if (signal == Signal::SHUTDOWN) {
		status = Status::FINISHED;
		std::unique_lock<std::mutex> mlock(mutex);
		mlock.unlock();
		condition.notify_one();

		//stops the worker thread
		if (worker.joinable())
			worker.join();

		//stop the EMGProvider
		emgProvider->send(Signal::SHUTDOWN);
	}
}

//TODO: no real-time plotting. Change this!
void Classifier::plot(Sample* sample, std::vector<math::Vector>& values) {
	if (config->isPlotRMS()) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(sample->getNumber())+"-rms.txt");
		sampleStream << *sample;
		sampleStream.close();
	}
	if (config->isPlotVariogramGraph()) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(sample->getNumber()) + "-graph.txt");
		for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); it++)
			sampleStream << it->getGroup() << "\t" << it->getLength(2) << "\t" << it->getZ() << std::endl;
		sampleStream.close();
	}
	if (config->isPlotVariogramSurface()) {
		std::ofstream sampleStream;
		sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(sample->getNumber()) + "-surface.txt");
		for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); it++)
			sampleStream << it->getX() << "\t" << it->getY() << "\t" << it->getZ() << std::endl;
		sampleStream.close();
	}
}