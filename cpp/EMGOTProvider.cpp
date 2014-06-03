
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "../h/EMGOTProvider.h"

EMGOTProvider::EMGOTProvider() {
	client = new OTBioLabClient("localhost");
};

EMGOTProvider::~EMGOTProvider() {
	send(Signal::STOP);
	delete client;
};

void EMGOTProvider::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (status == Status::NEW)
			thread = std::thread(&EMGOTProvider::run, this);
		if (status == Status::NEW || status == Status::WAITING) {
			status == Status::RUNNING;
			client->start();
		}
	}
	if (signal == Signal::SHUTDOWN) {
		status = Status::FINISHED;
		//only needed to unblock reading process
		client->getEMGgain();
		client->stop();
		if (thread.joinable())
			thread.join();
	}
	if (signal == Signal::STOP) {
		status = Status::WAITING;
		client->stop();
	}
}

void EMGOTProvider::run() {
	AppConfig *config = AppConfig::getInstance();
	int nrRows = config->getSampleRows();
	int nrColumns = config->getSampleColumns();
	long number = 0;
	while (true) {
		if (status == Status::FINISHED) {
			BOOST_LOG_TRIVIAL(info) << "Stopping EMGOTProvider";
			break;
		}
		std::vector<short> values;
		client->readChannels(values);
		Sample *s = new Sample(values, nrRows, nrColumns, number);
		++number;

		if (lastInterval == NULL)
			lastInterval = new Interval();
		lastInterval->addSample(s);
		if (lastInterval->isFull()) {
			BOOST_LOG_TRIVIAL(debug) << "created new Interval";
			EMGProvider::addInterval(lastInterval);
			EMGProvider::lastInterval = new Interval();
		}
	}
}