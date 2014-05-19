
#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/**
  * Defines the signals, that can be send to the EMGProvider and
  * the Classifier.
  * START starts the underlying worker Thread and STOP stops it.
  * It is possible to send this two signals alternating. SHUTDOWN
  * stops the underlying worker thread. It is not possible to start
  * it again. It is crucial to send STOP once the underlying worker
  * Thread has been started.
  */
enum Signal {
	START,
	STOP,
	SHUTDOWN
};


/**
* Defines the current status of the EMGProvider and the Classifier.
* NEW means the EMGProvider/Classifier was created, but never
* started. RUNNING means the EMGProvider/Classifier is currently
* processing requests. WAITING means the EMGProvider/Classifier is
* currently stoped, but it is possible to start it again. FINISHED
* means that the EMGProvider/Classifier has completed its work or
* was shutdown. It is not possible to start it again.
*/
enum Status {
	NEW,
	RUNNING,
	WAITING,
	FINISHED
};


#endif