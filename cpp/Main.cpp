#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include "../h/AppConfig.h"
#include "../h/Classifier.h"
#include "../h/Exception.h"
#include "../h/EMGFileProvider.h"
#include "../h/EMGOTProvider.h"
#include "../h/MultiClassSVM.h"
#include "../h/Plotter.h"
#include "../h/Trainer.h"
#include "../h/Utilities.h"

#include "../lib/OTBioLabClient/OTBioLabClient.h"


/**
  * It is necessary to pass an argument with the path to the configuration file.
  * The argument has to look like 'online_classificator.config <path_to_file>'
  * with a whitespace before the path.
  */
int main(int argc, char *argv[]) {
	//loads all configurations and inits the logging system
	try {
		AppConfig::load(argc, argv);
		AppConfig::getInstance()->initLogging();
	}
	catch (int ex) {
		if (ex == Exception::NO_CONFIGURATIONS_DEFINED)
			std::cerr << "Unable to find path to configuration file" << std::endl
			<< "Online Classificator usage: " << AppConfig::CONFIG_ARGUMENT << " <path_to_config>" << std::endl;
		if (ex == Exception::UNABLE_TO_OPEN_FILE)
			std::cerr << "Unable to read configurations from configuration file. Not possible to open the file." << std::endl;
		if (ex == Exception::UNABLE_TO_PARSE_CONFIGURATION)
			std::cerr << "Unable to parse configurations. Please check all configurations." << std::endl;
		return EXIT_FAILURE;
	}

	std::string path = "c:\\Users\\Marc\\Dropbox\\Informatik\\Studium\\6. Semester\\Bachelor Thesis\\MARC\\data\\data8_AN-format.txt";

	//It takes very much time to delete EMGProvider and/or Classifier.
	//Therefor I added this block, so that both are destroyed before the end
	//of the program is reached.
	{
		//EMGOTProvider emgProvider{};
		EMGFileProvider emgProvider{ path };
		MultiClassSVM svm;

		Trainer trainer(&emgProvider, &svm);
		//trainer.train();

		Classifier classifier(&emgProvider, &svm);
		classifier.send(Signal::START);
		Sleep(30000);
		classifier.send(Signal::SHUTDOWN);
	}

	//closes all open resources, releases heap memory, ...
	Plotter::release();
	AppConfig::release();

	//Only used to keep the terminal still visible, when the program is finished
	std::cout << "Please press Enter to close program... ";
	std::cin.get();
	return EXIT_SUCCESS;
}