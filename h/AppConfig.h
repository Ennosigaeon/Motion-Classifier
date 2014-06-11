#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <string>
#include "../lib/libsvm-3.18/svm.h"

/**
  * This class handles the application configuration. It is implemented as
  * a Singleton. To instantiate the instance you have to call static
  * AppConfig::load(const std::string&). You have to pass the path to the
  * configuration file as a command line argument. See AppConfig::CONFIG_ARGUMENT
  * for exact usage.
  * As an alternative you can call AppConfig::load(int argc, char *argv[])
  * to parse the path automatically and load all configurations.
  */
class AppConfig {
private:
	static AppConfig *instance;
	AppConfig();

	int emgProviderBufferWarning = 5;
	int intervalNrSamples = 1;
	std::string gnuPlotPath = "";
	bool plotRMS = 0;
	bool plotVariogramSurface = 0;
	bool plotVariogramGraph = 0;
	int sampleRows = 0;
	int sampleColumns = 0;
	int variogramNrBins = 1;
	int blockingQueueMaxWaitTime = -1;
	std::string trainerBaseDir = "";
	int trainingsSize = 1;
	int trainerNrRuns = 1;
	int loggerLevel = 2;
	std::string loggerFile = "";
	svm_parameter *param;

public:

	/**
	  * You have to pass a path to the configuration file as a commandline
	  * parameter. This parameter has to have to layout:
	  *    CONFIG_ARGUMENT /path/to/file
	  */
	static const std::string CONFIG_ARGUMENT;

	/**
	  * Get an instance of AppConfig. You have to call load(const std::string&)
	  * beforehand, otherwise the returned instance points to NULL.
	  */
	static AppConfig* getInstance();

	/**
	  * Loads all configurations from the given file and stores them in
	  * the AppConfig instance. When this function is called twice, the
	  * first configurations will be overriden.
	  * It is crucial to call release() after this function has been called
	  * once. Otherwise you have a memory leak.
	  * Throws	std::invalid_argument
	  */
	static void load(const std::string& path);

	/**
	  * Parses the command line arguments and searches for the CONFIG_ARGUMENT.
	  * Then all configurations are loaded from the given path. When this
	  * function is called twice, the first configurations will be overriden.
	  * It is crucial to call release() after this function has been called
	  * once. Otherwise you will create a memory leak.
	  * Throws	std::invalid_argument
	  */
	static void load(int argc, char *argv[]);

	/**
	  * Deletes the AppConfig instance and frees the allocated memory again. It
	  * is crucial to call this member function after load(const std::string&)
	  * has been called once.
	  */
	static void release();

	/**
	  * This member function initializes the Boost logging system. It sets
	  * the log format, creates a console appender/sink and, if defined in
	  * the configuration file, a file appender/sink.
	  */
	void initLogging();

	int getEMGProviderBufferWarning() const;
	int getIntervalNrSamples() const;
	const std::string& getGnuPlotPath() const;
	int getSampleRows() const;
	int getSampleColumns() const;
	int getVariogramNrBins() const;
	int getBlockingQueueMaxWaitTime() const;
	const std::string& getTrainerBaseDir() const;
	int getTrainingsSize() const;
	int getTrainerNrRuns() const;
	int getLoggerLevel() const;
	const std::string& getLoggerFile() const;
	bool isPlotRMS() const;
	bool isPlotVariogramSurface() const;
	bool isPlotVariogramGraph() const;
	svm_parameter* getSVMParameter() const;
};

#endif