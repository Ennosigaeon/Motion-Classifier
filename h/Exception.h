#ifndef EXCEPTION_H
#define EXCEPTION_h

/**
  * This enum contains all Errors the Motion-Classifier can throw.
  */
//TODO: review exception handling
enum  Exception {

	//Configuration contains wrong value
	UNABLE_TO_PARSE_CONFIGURATION,

	//No path to configuration file is defined.
	NO_CONFIGURATIONS_DEFINED,

	//Unable to open a file
	UNABLE_TO_OPEN_FILE,

	//The SVM has no data for at least on class
	SVM_MISSING_TRAININGS_DATA,

	//InputStream has reached the end of the file
	END_OF_FILE,

	//Traings data are too close to each other
	INVALID_TRAINING
};

#endif