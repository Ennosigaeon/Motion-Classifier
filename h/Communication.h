#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/**
  * Defines the signals, that can be send to the EMGProvider
  * and the Classifier.
  */
enum Signal {
	//Starts the underlying worker.
	START,

	//Stops the underlying worker.
	STOP,

	//Stops the underlying worker. It is not possible to restart
	//it again.
	SHUTDOWN
};

/**
  * Defines the current status of the EMGProvider, the
  * Trainer and the Classifier.
  */
enum Status {
	//Object is created but Worker never started.
	NEW,

	//Worker is currently running.
	RUNNING,

	//Worker is stoped. It is possible to restart it again.
	WAITING,

	//Worker is at least stoped, maybe also destroyed. It is not
	//possible to restart it again.
	FINISHED
};

#endif