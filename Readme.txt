
Motion-Classifier
written by Marc Zöller <mazoeller@gmail.com>


1. Introduction

This project aims to create an online classifier for EMG signals. The idea is it to record the EMG signals, process and evaluate them in real time. The result is a classification to one of nine movements. The project is written in C++.


2. Requirements

This project requires the following libraries:

2.1 Boost
Boost is a collection of libraries, to make the development with C++ easier. It is necessary for for actual project and for the OTBioLabCppClient-develop. See http://www.boost.org/ for more information and downloading instructions.

2.2 gnuplot-iostream
This library offers a simple connection between a C++ program and gnuplot. Only the header file gnuplot-iostream.h is needed. See http://www.stahlke.org/dan/gnuplot-iostream/ or https://gitorious.org/gnuplot-iostream/pages/Home for more information.

2.3 OTBioLabCppClient-develop
This code provides a way to read EMG signals from the OT BioLab software. It is available under https://github.com/RehabEngGroup/OTBioLabCppClient.

2.4 libsvm
This library provides an easy-to-use way to calculate Support Vector Machines. This library is used to do the classification task. See http://www.csie.ntu.edu.tw/~cjlin/libsvm/ for more information.

2.5 gnuplot (optional)
Gnuplot is an open source library for plotting. This library is optional. When a path to gnuplot is provided, the results will be plotted, otherwise the plotting will be ignored. See http://www.gnuplot.info/ for more information.
	

3. Usage

To use the classifier you have to provide a configuration file. See AppConfig for a detailed description. There are three important classes you have to deal with:
The first class is the EMGProvider. This class provides an abstract way to read an Interval of EMG signals. The actual reading part is implemented as an abstract member function. There are two implementations of this class, the EMGFileProvider and <BIOLAB_EMG_PROVIDER>. The EMGFileProvider reads the EMG signals from a file and the <BIOLAB_EMG_PROVIDER> reads the EMG signals from the <OT_BIOLAB_SOFTWARE>. It is possible to implement new ways of reading intervals by extending EMGProvider.
The second important class is the MultiClassSVM. This class offers an abstract way to classify the recorded signals. You have to provide trainings data for every single movement. Then calculate all Support Vector Machines.
The last class is the Classifier. This class expects an instance of a EMGProvider and a MultiClassSVM as parameters. It reads Intervals from the EMGProvider and classifies with the MultiClassSVM. Before the Classifier can be used, the MultiClassSVM has to be trained with pre classified EMG intervals. Otherwise the result will always be Motion::Muscle::UNKNOWN.


4. Configuration

To be able to use the classifier you have to provide a path to a configuration file. This path is passed by the command line parameter "motion_classifier.config" (this parameter may change, see AppConfig.h or AppConfig.cpp documentation for the latest parameter).
The configuration file has to have to following format: key = value \n. Empty lines and lines starting with a '#' will be skipped. The required keys are:

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
4.6 plot.variogramSurface (int/bool)
	Defines whether the Variogram should be plotted as a surface or not.
4.7 plot.variogramGraph (int/bool)
	Defines whether the Variogram should be plotted as a graph or not.
4.8 variogram.nrBins (int)
	Defines the number of bins in the variogram. A higher value results in less pairs.
4.9	blockingQueue.maxWaitTime (int)
	Defines the maximum wait time for the BlockingQueue in milliseconds. The default value is -1. It is recommended to change this value, otherwise you can get a deadlock!
4.10 trainer.baseDir (std::string)
	Defines the directory in which all trainings data will be stored.
4.11 trainer.trainingsSize (int)
	Defines the size of a trainings window.
4.12 logger.level (int)
	Defines the the log level. 0 means 'trace' and 5 means 'fatal'.
4.13 logger.file
	Defines a path to a file. All messages will be logged in this file. This property is optional.
4.14  svm.type (int)
	Defines which Support Vector Machine should be used. C_SVC = 0, NU_SVC = 1, ONE_CLASS = 2, EPSILON_SVR = 3, NU_SVR = 4. See libsvm Readme for more information.
4.15 svm.kernel (int)
	Defines which Kernel should be used. LINEAR = 0, POLY = 1, RBF = 2, SIGMOID = 3, PRECOMPUTED = 4. See libsvm Readme for more information.
4.16 svm.cost (int)
	Defines the costs for a mismatched item in the SVM training process.
	
		
5. Trouble Shooting

See known issues for description of known problems and their solutions.