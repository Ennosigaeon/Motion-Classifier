#include <boost/lexical_cast.hpp>
#include "../h/ClassifierImpl.h"
#include "../h/Utilities.h"


using namespace motion_classifier;

Motion::Muscle ClassifierImpl::getMuscleMotion() {
	Motion::Muscle *motion = lastMuscleMotion.pop();
	if (motion == NULL)
		return Motion::Muscle::UNKNOWN;
	else
		return *motion;
}

std::string ClassifierImpl::printStatistics() {
	std::string s("Classified " + boost::lexical_cast<std::string>(intervalCount)+" Samples in avg. " + boost::lexical_cast<std::string>(time / intervalCount) + " ms");
	return s;
}

void ClassifierImpl::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (status == Status::NEW) {
			//start EMGProvider
			emgProvider->send(Signal::START);

			//start worker thread
			status = Status::RUNNING;
			worker = std::thread(&ClassifierImpl::run, this);
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

void ClassifierImpl::run() {
	while (true) {
		if (status == Status::RUNNING) {
			logger->debug("waiting for new Interval");
			Interval *interval = emgProvider->getInterval();
			if (interval == NULL)
				continue;

			clock_t t = clock();
			//do the actual classification
			Motion::Muscle motion = classify(interval);
			//overrides the last stored value
			lastMuscleMotion.push(&motion);
			t = clock() - t;
			double tmp = ((double)t) / CLOCKS_PER_SEC * 1000;
			logger->info("classified new Interval in " + boost::lexical_cast<std::string>(tmp)+" ms as " + printMotion(motion));

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