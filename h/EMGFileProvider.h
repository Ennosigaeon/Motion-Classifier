
#ifndef EMGFILEPROVIDER_H
#define EMGFILEPROVIDER_H

#include <fstream>
#include "EMGProvider.h"

class EMGFileProvider : public EMGProvider {
	friend class EMGProvider;
private:
	std::ifstream fileIn;
	void run();
	int nrRows;
	int nrColumns;

public:
	EMGFileProvider(const std::string path);
	~EMGFileProvider();

	void send(const Signal& signal);
};

#endif