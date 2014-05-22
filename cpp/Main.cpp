#include <iostream>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include "../h/AppConfig.h"
#include "../h/Classifier.h"
#include "../h/Exception.h"
#include "../h/EMGFileProvider.h"
#include "../h/Plotter.h"

#include "../h/MultiClassSVM.h"


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

	MultiClassSVM svm;
	{
		for (int i = MuscleMotion::REST_POSITION; i < 4; i++) {
			std::vector<math::Vector> vector;
			for (int j = 0; j < 5; j++)
				vector.push_back(math::Vector(j, i * 10 + j));
			svm.train(static_cast<MuscleMotion>(i), vector);
		}
	}
	try {
		svm.calculateSVMs();
	}
	catch (int ex) {
		if (ex == Exception::SVM_MISSING_TRAININGS_DATA)
			BOOST_LOG_TRIVIAL(fatal) << "unable to calculate SVMs. Missing trainings data.";
		else
			throw ex;
	}

	for (int i = MuscleMotion::REST_POSITION; i < 4; i++) {
		std::vector<math::Vector> list;
		math::Vector vector(i, i * 10);
		list.push_back(vector);
		MuscleMotion result = svm.classify(list);
		std::cout << static_cast<MuscleMotion>(i) << " -> " << result << std::endl;
	}


	std::string path = "c:\\Users\\Marc\\Dropbox\\Informatik\\Studium\\6. Semester\\Bachelor Thesis\\MARC\\data\\data8_AN";

	//It takes very much time to delete EMGProvider and/or Classifier.
	//Therefor I added this block, so that both are destroyed before the end
	//of the program is reached.
	{
		//TODO: This is just dummy code. Add real application here
		EMGFileProvider emgProvider{ path };
		Classifier classifier(&emgProvider);
		classifier.send(Signal::START);
		Sleep(6000);
		classifier.send(Signal::SHUTDOWN);
		//TODO: Shutdown takes way more time, then it is supposed to do
	}

	//closes all open resources, releases heap memory, ...
	Plotter::release();
	AppConfig::release();

	//Only used to keep the terminal still visible, when the program is finished
	BOOST_LOG_TRIVIAL(info) << "Please press Enter to close program... ";
	std::cin.get();
	return EXIT_SUCCESS;
}