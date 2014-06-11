#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/EMGFileProvider.h"

EMGFileProvider::EMGFileProvider(const std::string& path) {
	fileIn.open(path, std::ios_base::in);
	if (!fileIn.is_open()) {
		BOOST_LOG_TRIVIAL(fatal) << "unable to read records from file. Stopping EMGFileProvider";
		throw std::invalid_argument("unable to open file");
	}
	lastInterval = new Interval();
	BOOST_LOG_TRIVIAL(info) << "EMGFileProvider created";
}

EMGFileProvider::~EMGFileProvider() {
	send(Signal::SHUTDOWN);
	BOOST_LOG_TRIVIAL(info) << "EMGFileProvider destroyed";
	//Intervals are deleted in EMGProvider destructor
}

void EMGFileProvider::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (EMGProvider::status == Status::NEW) {
			EMGProvider::status = Status::RUNNING;
			thread = std::thread(&EMGFileProvider::run, this);
		}
		else {
			EMGProvider::status = Status::RUNNING;
			std::unique_lock<std::mutex> mlock(mutex);
			mlock.unlock();
			condition.notify_one();
		}
	}
	if (signal == Signal::STOP)
		EMGProvider::status = Status::WAITING;
	if (signal == Signal::SHUTDOWN) {
		EMGProvider::status = Status::FINISHED;
		//release waiting thread
		std::unique_lock<std::mutex> mlock(mutex);
		mlock.unlock();
		condition.notify_one();
		//wait for worker to finish
		if (thread.joinable())
			thread.join();
		fileIn.close();
	}
}

//TODO: kontrollieren, ob die Werte in der richtigen Reihenfolge eingelesen werden.
void EMGFileProvider::run() {
	while (true) {
		if (EMGProvider::status == Status::FINISHED) {
			BOOST_LOG_TRIVIAL(info) << "shuting down EMGFileProvider worker";
			return;
		}
		if (EMGProvider::status == Status::WAITING) {
			BOOST_LOG_TRIVIAL(debug) << "EMGFileProvider stops reading from file";
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait(lk);
		}
		if (EMGProvider::status == Status::RUNNING) {
			Sample *s = new Sample(sampleNr);
			try {
				fileIn >> *s;
				lastInterval->addSample(s);
			}
			catch (int ex) {
				BOOST_LOG_TRIVIAL(warning) << "EMGFileProvider reached end of file. No more intervals will be read";
				status = Status::FINISHED;
			}
			++sampleNr;

			if (lastInterval->isFull()) {
				BOOST_LOG_TRIVIAL(debug) << "created new Interval";
				EMGProvider::addInterval(lastInterval);
				EMGProvider::lastInterval = new Interval();
			}
		}
	}
}