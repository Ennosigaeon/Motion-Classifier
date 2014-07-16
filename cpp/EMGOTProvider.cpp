#include "../h/EMGOTProvider.h"
#include "../h/Logger.h"

using namespace motion_classifier;

EMGOTProvider::EMGOTProvider() {
	Logger::getInstance()->info("Please start the Visualization in the OT BioLab. Press enter to continue...");
	std::cin.get();
	client = new OTBioLabClient("localhost");
	lastInterval = new Interval();
	//TODO: Test connection with exception handling;
};

EMGOTProvider::~EMGOTProvider() {
	send(Signal::SHUTDOWN);
	//When socket is closed before Visualization has been stoped, you have to restart OT BioLab
	Logger::getInstance()->info("Please stop the Visualization in the OT BioLab. Press enter to continue...");
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
	Logger *logger = Logger::getInstance();
	while (true) {
		if (status == Status::FINISHED) {
			logger->info("Stopping EMGOTProvider");
			break;
		}
		std::vector<short> values;
		client->readChannels(values);
		Sample *s = new Sample(values, sampleNr);
		++sampleNr;

		lastInterval->addSample(s);
		if (lastInterval->isFull()) {
			logger->debug("created new Interval");
			EMGProviderImpl::addInterval(lastInterval);
			EMGProviderImpl::lastInterval = new Interval();
		}
	}
}