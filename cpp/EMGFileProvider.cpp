#include "../h/EMGFileProvider.h"
#include "../h/Logger.h"

using namespace motion_classifier;

EMGFileProvider::EMGFileProvider(const std::string& path) {
	fileIn.open(path, std::ios_base::in);
	if (!fileIn.is_open()) {
		Logger::getInstance()->fatal("unable to read records from file. Stopping EMGFileProvider");
		throw std::invalid_argument("unable to open file");
	}
	lastInterval = new Interval();
	Logger::getInstance()->info("EMGFileProvider created");
}

EMGFileProvider::~EMGFileProvider() {
	send(Signal::SHUTDOWN);
	Logger::getInstance()->info("EMGFileProvider destroyed");
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
	Logger *logger = Logger::getInstance();
	while (true) {
		if (EMGProvider::status == Status::FINISHED) {
			logger->info("shuting down EMGFileProvider worker");
			return;
		}
		if (EMGProvider::status == Status::WAITING) {
			logger->debug("EMGFileProvider stops reading from file");
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
				logger->warn("EMGFileProvider reached end of file. No more intervals will be read");
				status = Status::FINISHED;
			}
			++sampleNr;

			if (lastInterval->isFull()) {
				logger->debug("created new Interval");
				EMGProviderImpl::addInterval(lastInterval);
				EMGProviderImpl::lastInterval = new Interval();
			}
		}
	}
}