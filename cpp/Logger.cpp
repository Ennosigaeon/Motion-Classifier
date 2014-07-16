#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include "../h/Logger.h"

Logger* Logger::instance = NULL;

Logger* Logger::getInstance() {
	if (instance == NULL)
		init(0, "");
	return instance;
}

void Logger::release() {
	delete instance;
}

void Logger::init(int level, std::string file) {
	namespace logging = boost::log;
	namespace keywords = boost::log::keywords;
	namespace expr = boost::log::expressions;

	logging::core::get()->set_filter(boost::log::trivial::severity >= level);
	logging::add_common_attributes();

	if (!file.empty()) {
		logging::add_file_log(
			keywords::file_name = file,
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
	Logger::instance = new Logger();
	instance->level = level;
}

Logger::Logger() {
	info("set up logging system");
}

void Logger::addLogFunction(void (*pointer)(std::string message)) {
	logger2 = pointer;
}

void Logger::log(int l, std::string message) {
	switch (l) {
	case 0:
		BOOST_LOG_TRIVIAL(trace) << message;
		break;
	case 1:
		BOOST_LOG_TRIVIAL(debug) << message;
		break;
	case 2:
		BOOST_LOG_TRIVIAL(info) << message;
		break;
	case 3:
		BOOST_LOG_TRIVIAL(warning) << message;
		break;
	case 4:
		BOOST_LOG_TRIVIAL(error) << message;
		break;
	case 5:
		BOOST_LOG_TRIVIAL(fatal) << message;
	}
	if (l >= level && logger2 != NULL)
		logger2(message);
}

void Logger::trace(std::string message) {
	log(boost::log::trivial::severity_level::trace, message);
}

void Logger::debug(std::string message) {
	log(boost::log::trivial::severity_level::debug, message);
}

void Logger::info(std::string message) {
	log(boost::log::trivial::severity_level::info, message);
}

void Logger::warn(std::string message) {
	log(boost::log::trivial::severity_level::warning, message);
}

void Logger::error(std::string message) {
	log(boost::log::trivial::severity_level::error, message);
}

void Logger::fatal(std::string message) {
	log(boost::log::trivial::severity_level::fatal, message);
}