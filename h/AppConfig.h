#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <string>
#include "../lib/libsvm-3.18/svm.h"

/**
  * This class handles the application configuration. It is implemented as a
  * Singleton. To instantiate the instance you have to call
  * static AppConfig::load(const std::string&). You have to pass the path to
  * the configuration file as a command line argument. See AppConfig::CONFIG_ARGUMENT
  * for exact usage.
  * As an alternative you can call AppConfig::load(int argc, char *argv[]) to parse
  * the path automatically and load all configurations.
  */
class AppConfig {
private:
	static AppConfig *instance;
	AppConfig();

	int intervalNrSamples;
	std::string gnuPlotPath;
	bool plotRMS;
	bool plotVariogramSurface;
	bool plotVariogramGraph;
	int sampleRows;
	int sampleColumns;
	int variogramNrBins;
	int blockingQueueMaxWaitTime;
	std::string trainerBaseDir;
	int loggerLevel;
	std::string loggerFile;
	svm_parameter *param;

public:
	static const std::string CONFIG_ARGUMENT;

	/**
	  * Get an instance of AppConfig. You have to call load(const std::string&)
	  * beforehand, otherwise the returned instance points to NULL.
	  */
	static AppConfig* getInstance();

	/**
	  * Loads all configurations from the given file and stores them in
	  * the AppConfig instance. When this function is called twice, the first
	  * configurations will be overriden.
	  * It is crucial to call release() after this function has been called
	  * once. Otherwise you have a memory leak.
	  * Throws Exception::UNABLE_TO_PARSE_CONFIGURATION,
	  * Exception::UNABLE_TO_OPEN_FILE
	  */
	static void load(const std::string& path);

	/**
	  * Parses the command line arguments and searches for the CONFIG_ARGUMENT.
	  * Then all configurations are loaded from the given path. When this
	  * function is called twice, the first configurations will be overriden.It
	  * is crucial to call release() after this function has been called once.
	  * Otherwise you will create a memory leak.
	  * Throws Exception::UNABLE_TO_PARSE_CONFIGURATION,
	  * Exception::UNABLE_TO_OPEN_FILE,
	  * Exception::NO_CONFIGURATIONS_DEFINED
	  */
	static void load(int argc, char *argv[]);

	/**
	  * Deletes the AppConfig instance and frees the allocated memory again. It
	  * is crucial to call this member function after load(const std::string&)
	  * has been called once.
	  */
	static void release();

	/**
	  * This member function initializes the Boost logging system. It sets the
	  * log format, creates a console appender/sink and, if defined in the
	  * configuration file, a file appender/sink.
	  */
	void initLogging();

	int getIntervalNrSamples() const;
	std::string getGnuPlotPath() const;
	int getSampleRows() const;
	int getSampleColumns() const;
	int getVariogramNrBins() const;
	int getBlockingQueueMaxWaitTime() const;
	std::string getTrainerBaseDir() const;
	int getLoggerLevel() const;
	std::string getLoggerFile() const;
	bool isPlotRMS() const;
	bool isPlotVariogramSurface() const;
	bool isPlotVariogramGraph() const;
	svm_parameter* getSVMParameter() const;
};

#endif