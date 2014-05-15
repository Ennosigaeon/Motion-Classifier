
#include "../h/Plotter.h"
#include "../h/AppConfig.h"

//sets the initial value of instance to NULL, due to forbidden 'in-class initialization'.
Plotter* Plotter::instance = NULL;

Plotter::Plotter() {
	gp = new Gnuplot(path);
	path = AppConfig::getInstance()->getGnuPlotPath();
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
	(*gp) << command << std::endl;
}

void Plotter::execute(const std::string& command, int windowNr) {
	setWindowNr(windowNr);
	(*gp) << command << std::endl;
	commit();
}

void Plotter::commit() {
	(*gp) << std::flush;
}

void Plotter::setWindowNr(int windowNr) {
	addToBatch("set term wxt " + boost::lexical_cast<std::string>(windowNr));
}
