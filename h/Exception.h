#ifndef EXCEPTION_H
#define EXCEPTION_h

/**
  * This enum contains all Errors the Motion-Classifier can throw.
  */
enum  Exception {
	UNABLE_TO_PARSE_CONFIGURATION,
	NO_CONFIGURATIONS_DEFINED,
	UNABLE_TO_OPEN_FILE
};

#endif