#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "../h/EMGOTProvider.h"

EMGOTProvider::EMGOTProvider() {
	BOOST_LOG_TRIVIAL(info) << "Please start the Visualization in the OT BioLab. Press enter to continue...";
	std::cin.get();
	client = new OTBioLabClient("localhost");
	lastInterval = new Interval();
	//TODO: Test connection with exception handling;
};

EMGOTProvider::~EMGOTProvider() {
	send(Signal::SHUTDOWN);
	//When socket is closed before Visualization has been stoped, you have to restart OT BioLab
	BOOST_LOG_TRIVIAL(info) << "Please stop the Visualization in the OT BioLab. Press enter to continue...";
	std::cin.get();
	delete client;
	//Intervals are deleted in EMGProvider destructor
};

void EMGOTProvider::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (status == Status::NEW)
			thread = std::thread(&EMGOTProvider::run, this);
		if (status == Status::NEW || status == Status::WAITING) {
			status = Status::RUNNING;
			client->start();
		}
	}
	if (signal == Signal::SHUTDOWN) {
		status = Status::FINISHED;
		client->shutdown();
		if (thread.joinable())
			thread.join();
	}
	if (signal == Signal::STOP) {
		status = Status::WAITING;
		client->stop();
	}
}

void EMGOTProvider::run() {
	while (true) {
		if (status == Status::FINISHED) {
			BOOST_LOG_TRIVIAL(info) << "Stopping EMGOTProvider";
			break;
		}
		std::vector<short> values;
		client->readChannels(values);
		Sample *s = new Sample(values, sampleNr);
		++sampleNr;

		lastInterval->addSample(s);
		if (lastInterval->isFull()) {
			BOOST_LOG_TRIVIAL(debug) << "created new Interval";
			EMGProvider::addInterval(lastInterval);
			EMGProvider::lastInterval = new Interval();
		}
	}
}