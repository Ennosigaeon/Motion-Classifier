
#include <iostream>
#include <sstream>
#include <thread>
#include <cmath>

#include "../h/AppConfig.h"
#include "../h/Classifier.h"
#include "../h/Exception.h"
#include "../h/Interval.h"
#include "../h/EMGFileProvider.h"
#include "../h/Plotter.h"
#include "../h/Sample.h"
#include "../h/Variogram.h"
#include "../h/Vector.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

void initLogging();

//It is necessary to pass an argument with the path to the configuration file.
//The argument has to look like 'online_classificator.config <path_to_file>'
//with a whitespace before the path
int main(int argc, char *argv[]) {
	try {
		AppConfig::load(argc, argv);
	}
	catch (int ex) {
		if (ex == Exception::NO_CONFIGURATIONS_DEFINED)
			std::cerr << "Unable to find path to configuration file" << std::endl
			<< "Online Classificator usage: " << AppConfig::CONFIG_ARGUMENT << " <path_to_config>" << std::endl;
		if (ex == Exception::UNABLE_TO_READ_CONFIGURATIONS)
			std::cerr << "Unable to read configurations from configuration file. Not possible to open the file." << std::endl;
		return EXIT_FAILURE;
	}

	//init logging system
	initLogging();
	BOOST_LOG_TRIVIAL(info) << "Main started";

	std::string path = "c:\\Users\\Marc\\Dropbox\\Informatik\\Studium\\6. Semester\\Bachelor Thesis\\MARC\\data\\data8_AN";

	{
		EMGFileProvider emgProvider{ path };
		Classifier classifier(&emgProvider);

		classifier.send(Signal::START);
		Sleep(10000);
		BOOST_LOG_TRIVIAL(info) << "stopping";
		classifier.send(Signal::SHUTDOWN);
	}

	//closes all open resources, releases heap memory, ...
	Plotter::release();
	AppConfig::release();

	//Only used to keep the terminal still visible, when the program is finished
	BOOST_LOG_TRIVIAL(info) << "Please press Enter to close program... ";
	std::cin.get();
	return EXIT_SUCCESS;
}

//inits the logging system
void initLogging() {
	namespace logging = boost::log;
	namespace keywords = boost::log::keywords;
	namespace expr = boost::log::expressions;
	AppConfig *config = AppConfig::getInstance();

	logging::core::get()->set_filter(boost::log::trivial::severity >= config->getLoggerLevel());
	logging::add_common_attributes();

	if (!config->getLoggerFile().empty()) {
		logging::add_file_log(
			keywords::file_name = config->getLoggerFile(),
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