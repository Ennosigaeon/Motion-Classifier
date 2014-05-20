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
	int nrRows;
	int nrColumns;
	std::mutex mutex;
	std::condition_variable condition;

public:

	/**
	  * Creates a new EMGFileProvider. The parameter has to
	  * point to a valid file. In this file the measured EMG
	  * signals are stored.
	  * Throws: Exception::UNABLE_TO_OPEN_FILE
	  */
	EMGFileProvider(const std::string path);

	/**
	  * Destroys the EMGFileProvider. The underlying worker
	  * thread will be stoped and destroyed.
	  */
	~EMGFileProvider();

	/**
	  * Sends a signal to the working thread.
	  * See EMGProvider for more information.
	  */
	void send(const Signal& signal);
};

#endif