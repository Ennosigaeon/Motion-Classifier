
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

	int intervalMaxSize;
	std::string gnuPlotPath;
	int emgFileProviderRows;
	int emgFileProviderColumns;

public:
	static const std::string CONFIG_ARGUMENT;

	/**
	  * Loads all configurations from the given file and stores them in
	  * the AppConfig instance. It is crucial to call release() after this
	  * function has been called once. Otherwise you have a memory leak.
	  */
	static void load(const std::string& path);

	/**
	  * Get an instance of AppConfig. You have to call load(const std::string&)
	  * beforehand, otherwise the returned instance points to NULL.
	  */
	static AppConfig* getInstance();

	/**
	  * Deletes the AppConfig instance and frees the allocated memory again. It
	  * is crucial to call this member function after load(const std::string&)
	  * has been called once.
	  */
	static void release();

	int getIntervalMaxSize() const;
	std::string getGnuPlotPath() const;
	int getEMGFileProviderRows() const;
	int getEMGFileProviderColumns() const;
};

#endif