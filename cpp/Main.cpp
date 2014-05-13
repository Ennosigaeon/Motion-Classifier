
#include <iostream>
#include <sstream>
#include <thread>
#include <cmath>

#include "../h/AppConfig.h"
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
	//parses the path to the configuration file from the command line parameters
	std::string configPath = "";
	for (int i = 0; i < argc; i++) {
		if (argv[i] == AppConfig::CONFIG_ARGUMENT && i < argc - 1) {
			configPath = argv[i + 1];
			break;
		}
	}
	if (configPath.empty()) {
		std::cerr << "Unable to find path to configuration file" << std::endl
			<< "Online Classificator usage: " << AppConfig::CONFIG_ARGUMENT << " <path_to_config>" << std::endl;
		return EXIT_FAILURE;
	}
	BOOST_LOG_TRIVIAL(info) << "using " << configPath << " as configuration file";
	try {
		AppConfig::load(configPath);
	}
	catch (int ex) {
		if (ex == Exception::UNABLE_TO_READ_CONFIGURATIONS)
			std::cerr << "Unable to read configurations from configuration file. Not possible to open the file." << std::endl;
	}

	//init logging system
	initLogging();
	BOOST_LOG_TRIVIAL(info) << "Main started";

	std::string path = "c:\\Users\\Marc\\Dropbox\\Informatik\\Studium\\6. Semester\\Bachelor Thesis\\MARC\\data\\data8_AN";
	EMGProvider *provider;
	{
		EMGFileProvider emgProvider{ "C:/Tmp/test.txt" };
		provider = &emgProvider;
		provider->send(Signal::START);

		for (int i = 0; i < 10; i++) {
			Interval *intv = provider->getInterval();
			Sample sample = intv->getRMSSample();

			std::ofstream sampleStream;
			sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(i)+".txt");
			sampleStream << sample;
			sampleStream.close();

			Variogram var;
			std::vector<math::Vector> values = var.calculate(sample);

			sampleStream.open(std::string("C:/Tmp/plot/") + boost::lexical_cast<std::string>(i)+"-result.txt");
			for (std::vector<math::Vector>::iterator it = values.begin(); it != values.end(); it++)
				sampleStream << it->getX() << "\t" << it->getY() << "\t" << it->getZ() << std::endl;
			sampleStream.close();

			delete intv;
		}
		provider->send(Signal::SHUTDOWN);
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