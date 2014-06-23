#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include "../h/AppConfig.h"

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
	if (instance == NULL)
		instance = new AppConfig();

	std::cout << "using " << path << " as configuration file" << std::endl;
	std::ifstream in(path);
	if (!in.is_open())
		throw std::invalid_argument("Unable to read configurations from configuration file. Not possible to open the file.");

	instance->prop.load(path);

	//add slash to end of trainerBaseDir
	std::string s = instance->prop.get("trainer.baseDir");
	char c = s.at(s.size() - 1);
	if (c != '\\' && c != '/')
		instance->prop.set("trainer.baseDir",  s + "/");
}

//inits the logging system
void AppConfig::initLogging() {
	namespace logging = boost::log;
	namespace keywords = boost::log::keywords;
	namespace expr = boost::log::expressions;

	logging::core::get()->set_filter(boost::log::trivial::severity >= prop.getInt("logger.level"));
	logging::add_common_attributes();

	if (!instance->prop.get("logger.file").empty()) {
		logging::add_file_log(
			keywords::file_name = prop.get("logger.file"),
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

	BOOST_LOG_TRIVIAL(info) << "set up logging system";
}

int AppConfig::getEMGProviderBufferWarning() const {
	return prop.getInt("blockingQueue.maxWaitTime");
}

int AppConfig::getSampleRows() const {
	return prop.getInt("sample.rows");
}

int AppConfig::getSampleColumns() const {
	return prop.getInt("sample.columns");
}

int AppConfig::getIntervalNrSamples() const {
	return prop.getInt("interval.nrSamples");
}

int AppConfig::getBlockingQueueMaxWaitTime() const {
	return prop.getInt("blockingQueue.maxWaitTime");
}

std::string AppConfig::getTrainerBaseDir() const {
	std::string s = prop.get("trainer.baseDir");
	return s;
}

int AppConfig::getTrainingsSize() const {
	return prop.getInt("trainer.trainingsSize");
}

int AppConfig::getTrainerNrRuns() const {
	return prop.getInt("trainer.nrRuns");
}
