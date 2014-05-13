
#include <fstream>
#include <iostream>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "../h/AppConfig.h"

//sets the initial value of instance to NULL, due to forbidden 'in-class initialization'.
AppConfig* AppConfig::instance = NULL;

//sets the value of CONFIG_ARGUMENT to "online_classificator.config"
const std::string AppConfig::CONFIG_ARGUMENT = "online_classificator.config";

AppConfig::AppConfig() {
	intervalMaxSize = -1;
	emgFileProviderColumns = -1;
	emgFileProviderRows = -1;
	gnuPlotPath = "";
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
	if (!in.is_open()) {
		//TODO: throw exception
	}

	std::string key = "";
	std::string value = "";
	std::string line;
	while (!in.eof()) {
		std::vector<std::string> values;
		std::getline(in, line);
		boost::split(values, line, boost::is_any_of("="), boost::token_compress_on);

		//TODO: continue here

		if (key.empty())
			std::getline(in, key);
		else {
			std::getline(in, value);

			//store value in correct variable (according to key)
			if (key == "emgFileProvider.rows")
				instance->emgFileProviderRows = boost::lexical_cast<int>(value);
			else {
				if (key == "emgFileProvider.columns")
					instance->emgFileProviderColumns = boost::lexical_cast<int>(value);
				else {
					if (key == "interval.maxSize")
						instance->intervalMaxSize = boost::lexical_cast<int>(value);
					else {
						if (key == "gnuPlot.path")
							instance->gnuPlotPath = value;
						else {
							//TODO: log unknown configuration
						}
					}
				}
			}
			key = std::string();
			value = std::string();
		}
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