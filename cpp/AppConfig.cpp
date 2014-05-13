
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "../h/AppConfig.h"
#include "../h/Exception.h"

//sets the initial value of instance to NULL, due to forbidden 'in-class initialization'.
AppConfig* AppConfig::instance = NULL;

//sets the value of CONFIG_ARGUMENT to "online_classifier.config"
const std::string AppConfig::CONFIG_ARGUMENT = "online_classifier.config";

AppConfig::AppConfig() {
	intervalMaxSize = 0;
	emgFileProviderColumns = 0;
	emgFileProviderRows = 0;
	variogramNrBins = 1;
	gnuPlotPath = "";
	loggerLevel = 0;
	loggerFile = "";
}

AppConfig* AppConfig::getInstance() {
	return instance;
}

void AppConfig::release() {
	if (instance != NULL)
		delete instance;
}

void AppConfig::load(const std::string& path) {
	if (instance == NULL)
		instance = new AppConfig();

	std::ifstream in(path);
	if (!in.is_open())
		throw Exception::UNABLE_TO_READ_CONFIGURATIONS;

	std::string line;
	while (!in.eof()) {
		std::vector<std::string> values;
		std::getline(in, line);
		boost::trim(line);

		//this line is a comment, ignore it
		if (line.empty() || line.at(0) == '#')
			continue;

		boost::split(values, line, boost::is_any_of("="), boost::token_compress_on);
		if (values.size() < 2)
			continue;
		boost::trim(values.at(0));
		boost::trim(values.at(1));

		//store value in correct variable (according to key)
		if (values.at(0) == "emgFileProvider.rows")
			instance->emgFileProviderRows = boost::lexical_cast<int>(values.at(1));
		if (values.at(0) == "emgFileProvider.columns")
			instance->emgFileProviderColumns = boost::lexical_cast<int>(values.at(1));
		if (values.at(0) == "interval.maxSize")
			instance->intervalMaxSize = boost::lexical_cast<int>(values.at(1));
		if (values.at(0) == "gnuPlot.path")
			instance->gnuPlotPath = values.at(1);
		if (values.at(0) == "variogram.nrBins")
			instance->variogramNrBins = boost::lexical_cast<int>(values.at(1));
		if (values.at(0) == "logger.level")
			instance->loggerLevel = boost::lexical_cast<int>(values.at(1));
		if (values.at(0) == "logger.file")
			instance->loggerFile = values.at(1);
	}
}

int AppConfig::getEMGFileProviderRows() const {
	return emgFileProviderRows;
}

int AppConfig::getEMGFileProviderColumns() const {
	return emgFileProviderColumns;
}

int AppConfig::getIntervalMaxSize() const {
	return intervalMaxSize;
}

std::string AppConfig::getGnuPlotPath() const {
	return gnuPlotPath;
}

int AppConfig::getVariogramNrBins() const {
	return variogramNrBins;
}

int AppConfig::getLoggerLevel() const {
	return loggerLevel;
}

std::string AppConfig::getLoggerFile() const {
	return loggerFile;
}