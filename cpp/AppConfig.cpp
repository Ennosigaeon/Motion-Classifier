#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include "../h/AppConfig.h"
#include "../h/Exception.h"

//sets the initial value of instance to NULL, due to forbidden 'in-class initialization'.
AppConfig* AppConfig::instance = NULL;

//sets the value of CONFIG_ARGUMENT to "motion_classifier.config"
const std::string AppConfig::CONFIG_ARGUMENT = "motion_classifier.config";

AppConfig::AppConfig() {
	intervalNrSamples = 1;
	sampleColumns = 0;
	sampleRows = 0;
	variogramNrBins = 1;
	gnuPlotPath = "";
	blockingQueueMaxWaitTime = -1;
	loggerLevel = 0;
	loggerFile = "";
	plotRMS = false;
	plotVariogramSurface = false;
	plotVariogramGraph = false;
}

AppConfig* AppConfig::getInstance() {
	return instance;
}

void AppConfig::release() {
	if (instance != NULL)
		delete instance;
}

void AppConfig::load(int argc, char *argv[]) {
	//parses the path to the configuration file from the command line parameters
	std::string configPath = "";
	for (int i = 0; i < argc; i++) {
		if (argv[i] == AppConfig::CONFIG_ARGUMENT && i < argc - 1) {
			configPath = argv[i + 1];
			break;
		}
	}
	if (configPath.empty())
		throw NO_CONFIGURATIONS_DEFINED;
	std::cout << "using " << configPath << " as configuration file" << std::endl;
	AppConfig::load(configPath);
}

void AppConfig::load(const std::string& path) {
	if (instance == NULL)
		instance = new AppConfig();

	std::ifstream in(path);
	if (!in.is_open())
		throw Exception::UNABLE_TO_OPEN_FILE;

	std::string line;
	while (!in.eof()) {
		std::vector<std::string> values;
		std::getline(in, line);
		boost::trim(line);

		//the line is empty or a comment, ignore it
		if (line.empty() || line.at(0) == '#')
			continue;

		//The boost::split method creates a msvc warning. See Problems.txt->Unsafe string copy
		boost::split(values, line, boost::is_any_of("="), boost::token_compress_on);
		if (values.size() < 2)
			continue;
		boost::trim(values.at(0));
		boost::trim(values.at(1));

		//store value in correct variable (according to key)
		try {
			if (values.at(0) == "sample.rows")
				instance->sampleRows = boost::lexical_cast<int>(values.at(1));
			if (values.at(0) == "sample.columns")
				instance->sampleColumns = boost::lexical_cast<int>(values.at(1));
			if (values.at(0) == "interval.nrSamples")
				instance->intervalNrSamples = boost::lexical_cast<int>(values.at(1));
			if (values.at(0) == "gnuPlot.path")
				instance->gnuPlotPath = values.at(1);
			if (values.at(0) == "plot.rms")
				instance->plotRMS = boost::lexical_cast<bool>(values.at(1));
			if (values.at(0) == "plot.variogramSurface")
				instance->plotVariogramSurface = boost::lexical_cast<bool>(values.at(1));
			if (values.at(0) == "plot.variogramGraph")
				instance->plotVariogramGraph = boost::lexical_cast<bool>(values.at(1));
			if (values.at(0) == "variogram.nrBins")
				instance->variogramNrBins = boost::lexical_cast<int>(values.at(1));
			if (values.at(0) == "blockingQueue.maxWaitTime")
				instance->blockingQueueMaxWaitTime = boost::lexical_cast<int>(values.at(1));
			if (values.at(0) == "logger.level")
				instance->loggerLevel = boost::lexical_cast<int>(values.at(1));
			if (values.at(0) == "logger.file")
				instance->loggerFile = values.at(1);
		}
		catch (boost::bad_lexical_cast &) {
			throw Exception::UNABLE_TO_PARSE_CONFIGURATION;
		}
	}
}

//inits the logging system
void AppConfig::initLogging() {
	namespace logging = boost::log;
	namespace keywords = boost::log::keywords;
	namespace expr = boost::log::expressions;

	logging::core::get()->set_filter(boost::log::trivial::severity >= loggerLevel);
	logging::add_common_attributes();

	if (!loggerFile.empty()) {
		logging::add_file_log(
			keywords::file_name = loggerFile,
			keywords::rotation_size = 10 * 1024 * 1024,
			keywords::format = (
			expr::stream
			<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f")
			<< ": <" << logging::trivial::severity
			<< "> " << expr::smessage
			)
			);
	}
	logging::add_console_log(
		std::cout,
		keywords::format = (
		expr::stream
		<< expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f")
		<< ": <" << logging::trivial::severity
		<< "> " << expr::smessage
		)
		);
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

bool AppConfig::isPlotRMS() const {
	//no plotting possible, when no path to gnuplot is provided
	return plotRMS & !gnuPlotPath.empty();
}

bool AppConfig::isPlotVariogramSurface() const {
	//no plotting possible, when no path to gnuplot is provided
	return plotVariogramSurface && !gnuPlotPath.empty();
}

bool AppConfig::isPlotVariogramGraph() const {
	//no plotting possible, when no path to gnuplot is provided
	return plotVariogramGraph && !gnuPlotPath.empty();
}

int AppConfig::getBlockingQueueMaxWaitTime() const {
	return blockingQueueMaxWaitTime;
}