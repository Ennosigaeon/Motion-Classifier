
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <string>

/**
  * This class handles the application configuration. It is implemented as a
  * Singleton. To instantiate the instance you have to call
  * static AppConfig::load(const std::string&). You have to pass the path to
  * the configuration file as a command line argument. See AppConfig::CONFIG_ARGUMENT
  * for exact usage.
  */
class AppConfig {
private:
	static AppConfig *instance;
	AppConfig();

	int intervalNrSamples;
	std::string gnuPlotPath;
	bool plotRMS;
	bool plotVariogram;
	int sampleRows;
	int sampleColumns;
	int variogramNrBins;
	int blockingQueueMaxWaitTime;
	int loggerLevel;
	std::string loggerFile;

public:
	static const std::string CONFIG_ARGUMENT;

	/**
	* Get an instance of AppConfig. You have to call load(const std::string&)
	* beforehand, otherwise the returned instance points to NULL.
	*/
	static AppConfig* getInstance();

	/**
	  * Loads all configurations from the given file and stores them in
	  * the AppConfig instance. It is crucial to call release() after this
	  * function has been called once. Otherwise you have a memory leak.
	  * Throws Exception::UNABLE_TO_READ_CONFIGURATIONS
	  */
	static void load(const std::string& path);

	/**
	  * Parses the command line arguments and searches for the CONFIG_ARGUMENT.
	  * Then all configurations are loaded from the given path. It is crucial
	  * to class release() after this function has been called once. Otherwise
	  * you will create a memory leak.
	  * Throws Exception::UNABLE_TO_READ_CONFIGURATIONS,
	  * Exception::NO_CONFIGURATIONS_DEFINED
	  */
	static void load(int argc, char *argv[]);

	/**
	  * Deletes the AppConfig instance and frees the allocated memory again. It
	  * is crucial to call this member function after load(const std::string&)
	  * has been called once.
	  */
	static void release();

	int getIntervalNrSamples() const;
	std::string getGnuPlotPath() const;
	int getSampleRows() const;
	int getSampleColumns() const;
	int getVariogramNrBins() const;
	int getBlockingQueueMaxWaitTime() const;
	int getLoggerLevel() const;
	std::string getLoggerFile() const;
	bool isPlotRMS() const;
	bool isPlotVariogram() const;
};

#endif