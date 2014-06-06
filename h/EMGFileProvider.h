#ifndef EMGFILEPROVIDER_H
#define EMGFILEPROVIDER_H

#include <condition_variable>
#include <fstream>
#include <mutex>
#include "EMGProvider.h"

/**
  * This class is one implementation of the EMGProvider.
  * It reads the EMG signals from a file and parses them
  * into an interval.
  * See EMGProvider for more information.
  */
class EMGFileProvider : public EMGProvider {
	friend class EMGProvider;
private:
	std::ifstream fileIn;
	void run();
	std::mutex mutex;
	std::condition_variable condition;

public:

	/**
	  * Creates a new EMGFileProvider. The parameter has to
	  * be a path to a valid file. This file has to contain
	  * EMG values. See convertFile() in Utilities.h for
	  * more information about the exact format of the file.
	  * Throws	Exception::UNABLE_TO_OPEN_FILE
	  */
	EMGFileProvider(const std::string& path);

	/**
	  * Destroys the EMGFileProvider. All open resources
	  * will be closed.
	  */
	~EMGFileProvider();

	/**
	  * Sends a signal to the worker thread. See EMGProvider
	  * for more information.
	  */
	void send(const Signal& signal);
};

#endif