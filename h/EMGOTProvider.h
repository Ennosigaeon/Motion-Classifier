
#ifndef OTEMGPROVIDER_H
#define OTEMGPROVIDER_H

#include "EMGProvider.h"
#include "../lib/OTBioLabClient/OTBioLabClient.h"


class EMGOTProvider : public EMGProvider {
	friend class EMGProvider;
private:
	OTBioLabClient *client;
	void run();

public:
	EMGOTProvider();

	~EMGOTProvider();

	void send(const Signal& signal);

};


#endif