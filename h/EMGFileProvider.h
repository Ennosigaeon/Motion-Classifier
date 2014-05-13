
#ifndef EMGFILEPROVIDER_H
#define EMGFILEPROVIDER_H

#include <fstream>
#include "EMGProvider.h"

class EMGFileProvider : public EMGProvider {
	friend class EMGProvider;
private:
	std::ifstream fileIn;
	void run();
	//TODO: make configurable
	int nrRows = 8;
	int nrColumns = 24;

public:
	EMGFileProvider(const std::string path);
	~EMGFileProvider();

	void send(const Signal& signal);
};

#endif