#ifndef OTEMGPROVIDER_H
#define OTEMGPROVIDER_H

#include "EMGProvider.h"
#include "../lib/OTBioLabClient/OTBioLabClient.h"

/**
  * This class is a implementation of the EMGProvider. It
  * reads EMG sensor values from the OT BioLab software.
  * See the Readme file for a detailed description how to
  * configure the OT BioLab software. See also EMGProvider
  * for more information.
  */
class EMGOTProvider : public EMGProvider {
	friend class EMGProvider;
private:
	OTBioLabClient *client;
	void run();

public:
	/**
	  * Creates a new EMGOTProvider. A connection to the
	  * OT BioLab software is established. Therefore it
	  * is important that you do not run several instances
	  * of the EMGOTProvider at the same time.
	  */
	EMGOTProvider();

	/**
	  * Destroys the EMGOTProvider. The connection to the
	  * OT BioLab software is closed. See Readme for
	  * more information how to use the EMGOTProvider.
	  */
	~EMGOTProvider();

	/**
	  * Sends a signal to the worker. See EMGProvider
	  * for more information.
	  */
	void send(const Signal& signal);
};

#endif