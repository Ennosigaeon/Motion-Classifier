
#include <sstream>
#include <thread>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "../h/EMGFileProvider.h"
#include "../h/AppConfig.h"

EMGFileProvider::EMGFileProvider(const std::string path) {
	AppConfig *conf = AppConfig::getInstance();
	nrRows = conf->getEMGFileProviderRows();
	nrColumns = conf->getEMGFileProviderColumns();

	EMGFileProvider::fileIn.open(path, std::ios_base::in);
	BOOST_LOG_TRIVIAL(info) << "EMGFileProvider created";
}

EMGFileProvider::~EMGFileProvider() {
	if (thread.joinable())
		thread.join();
	EMGFileProvider::fileIn.close();
	BOOST_LOG_TRIVIAL(info) << "EMGFileProvider destroyed";
}

void EMGFileProvider::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (EMGProvider::status == RUNNING)
			; //TODO: Notify thread
		else {
			thread = std::thread(&EMGFileProvider::run, this);
			EMGProvider::status = Status::RUNNING;
		}
	}
	if (signal == Signal::STOP)
		EMGProvider::status = Status::WAITING;
	if (signal == Signal::SHUTDOWN) {
		EMGProvider::status = Status::FINISHED;
		//TOOD: Notify thread
	}
}

//TODO: kontrollieren, ob die Werte in der richtigen Reihenfolge eingelesen werden.
void EMGFileProvider::run() {
	while (true) {
		if (EMGProvider::status == Status::FINISHED) {
			BOOST_LOG_TRIVIAL(debug) << "shuting down EMGFileProvider";
			break;
		}
		if (EMGProvider::status == Status::WAITING)
			;//TODO: Wait for change of status
		if (EMGProvider::status == Status::RUNNING) {
			if (!EMGFileProvider::fileIn.is_open()) {
				//TODO: replace with exception
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
				//TODO: what happens when reading fails (EOF)??
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
