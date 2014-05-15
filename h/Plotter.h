
#ifndef PLOTTER_H
#define PLOTTER_H

#include <iostream>
#include <string>

#include <boost/lexical_cast.hpp>
#include "../lib/gnuplot-iostream.h"

/**
  * This class handles all plotting commands. It uses gnuplot-iostream to send all plotting
  * commands to gnuplot.
  */
//TODO: Plotter is never used
class Plotter {
private:
	static Plotter *instance;
	std::string path = "";
	Gnuplot* gp;

	Plotter();
	~Plotter();

public:

	/**
	  * Get an instance of the Plotter. It is crucial to call release once getInstance() was called.
	  */
	static Plotter* getInstance();

	/**
	  * It is crucial to call this function when the Plotter isn't needed anymore. Otherwise this is a
	  * memory leak. Calling this function will also close all open plotting windows.
	  */
	static void release();

	/**
	  * Sends the given command to the plotter. The commands are cached. It is necessary to call commit()
	  * to see the results.
	  */
	void addToBatch(const std::string& command);

	/**
	  * Sends the given command to the plotter and commits immedeatly. The command will be executed in
	  * the given window number. The default value is 0.
	  * Important: When the window number is already used and the window is still open the previous
	  * plot in this window will be overriden.
	  */
	void execute(const std::string& command, int windowNr = 0);

	/**
	  * Commits all cached commands; The commands will be commited to the given window number. The default
	  * value is 0.
	  * Important: When the window number is already used and the window is still open the previous
	  * plot in this window will be overriden.
	  */
	void commit();

	/**
	  * Sets the current window. It is necessary to call this function before starting a new batch of
	  * commands.
	  */
	void setWindowNr(int windowNr);
};

#endif