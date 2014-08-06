#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include "../h/AppConfig.h"
#include "../h/Logger.h"

using namespace motion_classifier;

//sets the initial value of instance to NULL, due to forbidden 'in-class initialization'.
AppConfig* AppConfig::instance = NULL;

//sets the value of CONFIG_ARGUMENT to "motion_classifier.config"
const std::string AppConfig::CONFIG_ARGUMENT = "motion_classifier.config";

AppConfig::AppConfig() {
}

AppConfig* AppConfig::getInstance() {
	return instance;
}

void AppConfig::release() {
	if (instance != NULL) {
		delete instance;
	}
}

void AppConfig::load(int argc, char *argv[]) {
	//parses the path to the configuration file from the command line parameters
	std::string configPath = "";
	for (int i = 0; i < argc; ++i) {
		if (argv[i] == AppConfig::CONFIG_ARGUMENT && i < argc - 1) {
			configPath = argv[i + 1];
			break;
		}
	}
	if (configPath.empty())
		throw std::invalid_argument("no path to configuration defined.");
	AppConfig::load(configPath);
}

void AppConfig::load(const std::string& path) {
	std::cout << "using " << path << " as configuration file" << std::endl;
	std::ifstream in(path);
	if (!in.is_open())
		throw std::invalid_argument("Unable to read configurations from configuration file. Not possible to open the file.");
	Properties prop(path);
	load(prop);
}

void AppConfig::load(Properties& prop) {
	if (instance == NULL)
		instance = new AppConfig();

	instance->blockingQueueMaxWaitTime = prop.getInt("blockingQueue.maxWaitTime");
	instance->sampleRows = prop.getInt("sample.rows");
	instance->sampleColumns = prop.getInt("sample.columns");
	instance->intervalNrSamples = prop.getInt("interval.nrSamples");
	instance->emgProviderBufferWarning = prop.getInt("emgProvider.bufferWarning");
	instance->trainingsSize = prop.getInt("trainer.trainingsSize");
	instance->trainerNrRuns = prop.getInt("trainer.nrRuns");

	
	//add slash to end of trainerBaseDir
	instance->trainerBaseDir = prop.get("trainer.baseDir");
	char c = instance->trainerBaseDir.at(instance->trainerBaseDir.size() - 1);
	if (c != '\\' && c != '/')
		instance->trainerBaseDir += "/";

	Logger::init(prop.getInt("logger.level"), prop.get("logger.file"));
}

int AppConfig::getEMGProviderBufferWarning() const {
	return emgProviderBufferWarning;
}

int AppConfig::getSampleRows() const {
	return sampleRows;
}

int AppConfig::getSampleColumns() const {
	return sampleColumns;
}

int AppConfig::getIntervalNrSamples() const {
	return intervalNrSamples;
}

int AppConfig::getBlockingQueueMaxWaitTime() const {
	return blockingQueueMaxWaitTime;
}

std::string AppConfig::getTrainerBaseDir() const {
	return trainerBaseDir;
}

int AppConfig::getTrainingsSize() const {
	return trainingsSize;
}

int AppConfig::getTrainerNrRuns() const {
	return trainerNrRuns;
}

