
#include <sstream>
#include <thread>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "../h/EMGFileProvider.h"
#include "../h/AppConfig.h"

EMGFileProvider::EMGFileProvider(const std::string path) {
	AppConfig *conf = AppConfig::getInstance();
	nrRows = conf->getSampleRows();
	nrColumns = conf->getSampleColumns();

	EMGFileProvider::fileIn.open(path, std::ios_base::in);
	BOOST_LOG_TRIVIAL(info) << "EMGFileProvider created";
}

EMGFileProvider::~EMGFileProvider() {
	send(Signal::SHUTDOWN);
	EMGFileProvider::fileIn.close();
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
	while (true) {
		if (EMGProvider::status == Status::FINISHED) {
			BOOST_LOG_TRIVIAL(info) << "shuting down EMGFileProvider";
			//TODO: notify that no more intervals will be created
			break;
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
			Sample s{ nrRows, nrColumns };
			double x = 0, y = 0;
			while (x < s.getNrColumns()) {
				math::Vector entry(x, y, 0);
				std::string tmp;
				fileIn >> tmp;

				//convert string into number
				std::istringstream converter(tmp);
				double number;
				if (!(converter >> number))
					entry.setZ(NAN);
				else
					entry.setZ(number);
				s.addEntry(entry);
				y++;
				if (y == s.getNrRows()) {
					y = 0;
					x++;
				}

				//stop reading when eof is reached
				if (fileIn.eof()) {
					BOOST_LOG_TRIVIAL(warning) << "EMGFileProvider reached end of file. No more intervals will be read";
					status = Status::FINISHED;
					break;
				}
			}

			lastInterval->addSample(s);
			if (lastInterval->isFull()) {
				BOOST_LOG_TRIVIAL(debug) << "created new Interval";
				EMGProvider::addInterval(lastInterval);
				EMGProvider::lastInterval = new Interval();
			}
		}
	}
}
