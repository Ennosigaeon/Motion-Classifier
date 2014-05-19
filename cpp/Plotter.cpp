
#include "../h/Plotter.h"
#include "../h/AppConfig.h"

//sets the initial value of instance to NULL, due to forbidden 'in-class initialization'.
Plotter* Plotter::instance = NULL;

Plotter::Plotter() {
	path = AppConfig::getInstance()->getGnuPlotPath();
	if (!path.empty())
		gp = new Gnuplot(path);
}

Plotter::~Plotter() {
	delete gp;
}

Plotter* Plotter::getInstance() {
	if (instance == NULL)
		instance = new Plotter();
	return instance;
}

void Plotter::release() {
	if (instance != NULL)
		delete instance;
}

void Plotter::addToBatch(const std::string& command) {
	if (gp != NULL)
		(*gp) << command << std::endl;
}

void Plotter::execute(const std::string& command, int windowNr) {
	if (gp != NULL) {
		setWindowNr(windowNr);
		(*gp) << command << std::endl;
		commit();
	}
}

void Plotter::commit() {
	if (gp != NULL)
		(*gp) << std::flush;
}

void Plotter::setWindowNr(int windowNr) {
	addToBatch("set term wxt " + boost::lexical_cast<std::string>(windowNr));
}
