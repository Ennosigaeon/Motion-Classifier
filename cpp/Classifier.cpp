
#include <iostream>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/Classifier.h"

Classifier::Classifier(EMGProvider* emgProvider) {
	Classifier::emgProvider = emgProvider;
	BOOST_LOG_TRIVIAL(info) << "Classifier created";
}

Classifier::~Classifier() {
	send(Signal::SHUTDOWN);
	BOOST_LOG_TRIVIAL(info) << "Classifier destroyed";
}

void Classifier::train(const MuscleMotion& motion, std::vector<math::Vector>& data) {
	svm.train(motion, data);
}


MuscleMotion Classifier::getMuscleMotion() {
	MuscleMotion *motion = lastMuscleMotion.pop();
	if (motion == NULL)
		return MuscleMotion::UNKNOWN;
	else
		return *motion;
}

void Classifier::run() {
	//TODO: remove
	int i = 0;
	while (true) {
		if (status == Status::RUNNING) {
			BOOST_LOG_TRIVIAL(debug) << "waiting for new Interval";
			Interval *interval = emgProvider->getInterval();
			if (interval == NULL)
				continue;

			BOOST_LOG_TRIVIAL(debug) << "calculationg RMS sample";
			Sample rms = interval->getRMSSample();

			BOOST_LOG_TRIVIAL(debug) << "calculation Variogram";
			std::vector<math::Vector> values = variogram.calculate(rms);

			BOOST_LOG_TRIVIAL(debug) << "classifying values";
			MuscleMotion motion = svm.classify(values);

			//TODO: remove
			std::ofstream sampleStream;
			sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(i)+".txt");
			sampleStream << rms;
			sampleStream.close();

			sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(i)+"-result.txt");
			for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); it++)
				sampleStream << it->getX() << "\t" << it->getY() << "\t" << it->getZ() << std::endl;
			sampleStream.close();
			i++;

			//overrides the last stored value
			lastMuscleMotion.push(&motion);
			BOOST_LOG_TRIVIAL(info) << "classified new Interval";
			delete interval;
		}
		if (status == Status::WAITING) {
			BOOST_LOG_TRIVIAL(debug) << "Classifier stops processing Intervals";
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait(lk);
		}
		if (status == Status::FINISHED) {
			BOOST_LOG_TRIVIAL(info) << "shuting down Classifier";
			break;
		}
	}
}

void Classifier::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (status == Status::NEW) {
			//starts EMGProvider
			emgProvider->send(Signal::START);

			//starts worker thread
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