
Motion-Classifier
written by Marc Zöller <mazoeller@gmail.com>


1. Introduction

This project aims to create an online classifier for EMG signals. The idea is it to record the EMG signals, process and evaluate them in real time. The result is a classification to one of nine movements. The project is written in C++.


2. Requirements

This project requires the following libraries:

2.1 Boost
Boost is a collection of libraries, to make the development with C++ easier. It is necessary for for the actual project and for the OTBioLabCppClient-develop. See http://www.boost.org/ for more information and downloading instructions.

2.2 gnuplot-iostream
This library offers a simple connection between a C++ program and gnuplot. Only the header file gnuplot-iostream.h is needed. See http://www.stahlke.org/dan/gnuplot-iostream/ or https://gitorious.org/gnuplot-iostream/pages/Home for more information.

2.3 OTBioLabCppClient-develop
This code provides a way to read EMG signals from the OT BioLab software. It is available under https://github.com/RehabEngGroup/OTBioLabCppClient.

2.4 gnuplot (optional)
Gnuplot is an open source library for plotting. This library is optional. When a path to gnuplot is provided, the results will be plotted, otherwise the plotting will be ignored. See http://www.gnuplot.info/ for more information.
	

3. Usage

To use the classifier you have to provide a configuration file. See Configuration for a detailed description. There are two important classes. The first class is the EMGProvider. This class provides an abstract way to read an Interval of EMG signals. The actual reading part is implemented as an abstract member function. There are two implementations of this class, the EMGFileProvider and <BIOLAB_EMG_PROVIDER>. The EMGFileProvider reads the EMG signals from a file and the <BIOLAB_EMG_PROVIDER> reads the EMG signals from the <OT_BIOLAB_SOFTWARE>. It is possible to implement new ways of reading intervals by extending EMGProvider.
The second important class is <CLASSIFIER>. This class gets an EMGProvider as a creation parameter. It reads Intervals from the EMGProvider and classifies them. Before the <CLASSIFIER> can be used it has to be trained with pre classified EMG intervals.
	
4. Configuration

To be able to use the classifier you have to provide a path to a configuration file. This path is passed by the command line parameter "motion_classifier.config" (this parameter can maybe change, see AppConfig.h/AppConfig.cpp documentation for the latest path).
The configuration file has to have to following format: key = value \n. The required keys are:

4.1 sample.rows (int)
	Defines the number of rows in a single sample.
4.2 sample.columns (int)
	Defines the number of columns in a single sample.
4.3 interval.nrSamples (int)
	Defines the number of samples a single interval contains.
4.4 gnuPlot.path (std::string)
	Defines the path to the gnuplot executable. This property is optional. When it is not set, plotting will be deactivated.
4.5 plot.rms (int/bool)
	Defines whether the RMS sample should be plotted or not.
4.6 plot.variogram (int/bool)
	Defines whether the Variogram should be plotted or not.
4.7 variogram.nrBins (int)
	Defines the number of bins in the variogram. A higher value results in less pairs.
4.8 logger.level (int)
	Defines the the log level. 0 means 'trace' and 5 means 'fatal'.
4.9 logger.file
	Defines a path to a file. All messages will be logged in this file. This property is optional.
		
5. Trouble Shooting

See known issues for description of known problems and their solutions.