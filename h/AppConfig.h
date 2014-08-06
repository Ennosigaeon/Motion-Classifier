#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "Properties.h"


namespace motion_classifier {
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

		int emgProviderBufferWarning;
		int intervalNrSamples;
		int sampleRows;
		int sampleColumns;
		int blockingQueueMaxWaitTime;
		std::string trainerBaseDir;
		int trainingsSize;
		int trainerNrRuns;

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

		/*
		 * Initializes the application configuration from the given properties file.
		 * It is crucial to call release() after this function has been called
		 * once. Otherwise you will create a memory leak.
		 */
		static void load(Properties& prop);

		/**
		  * Deletes the AppConfig instance and frees the allocated memory again. It
		  * is crucial to call this member function after load(const std::string&)
		  * has been called once.
		  */
		static void release();

		int getEMGProviderBufferWarning() const;
		int getIntervalNrSamples() const;
		int getSampleRows() const;
		int getSampleColumns() const;
		int getBlockingQueueMaxWaitTime() const;
		std::string getTrainerBaseDir() const;
		int getTrainingsSize() const;
		int getTrainerNrRuns() const;
	};
}

#endif