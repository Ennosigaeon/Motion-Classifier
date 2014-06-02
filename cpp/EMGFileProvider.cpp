#include <sstream>
#include <thread>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/EMGFileProvider.h"
#include "../h/Exception.h"
#include "../h/AppConfig.h"

EMGFileProvider::EMGFileProvider(const std::string& path) {
	AppConfig *conf = AppConfig::getInstance();
	nrRows = conf->getSampleRows();
	nrColumns = conf->getSampleColumns();

	fileIn.open(path, std::ios_base::in);
	if (!fileIn.is_open())
		throw Exception::UNABLE_TO_OPEN_FILE;
	BOOST_LOG_TRIVIAL(info) << "EMGFileProvider created";
}

EMGFileProvider::~EMGFileProvider() {
	send(Signal::SHUTDOWN);
	fileIn.close();
	BOOST_LOG_TRIVIAL(info) << "EMGFileProvider destroyed";
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
		std::unique_lock<std::mutex> mlock(mutex);
		mlock.unlock();
		condition.notify_one();
		if (thread.joinable())
			thread.join();
	}
}

//TODO: kontrollieren, ob die Werte in der richtigen Reihenfolge eingelesen werden.
void EMGFileProvider::run() {
	long number = 0;
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
			if (!EMGFileProvider::fileIn.is_open()) {
				BOOST_LOG_TRIVIAL(fatal) << "unable to read records from file. Stopping EMGFileProvider";
				return;
			}
			if (lastInterval == NULL)
				lastInterval = new Interval();

			//sensor array of 8 rows and 24 columns, in total 192 values
			Sample *s = new Sample{ nrRows, nrColumns, number };
			try {
				fileIn >> *s;
				lastInterval->addSample(s);
			}
			catch (int ex) {
				BOOST_LOG_TRIVIAL(warning) << "EMGFileProvider reached end of file. No more intervals will be read";
				status = Status::FINISHED;
			}
			number++;
			
			if (lastInterval->isFull()) {
				BOOST_LOG_TRIVIAL(debug) << "created new Interval";
				EMGProvider::addInterval(lastInterval);
				EMGProvider::lastInterval = new Interval();
			}
		}
	}
}