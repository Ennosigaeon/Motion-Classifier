
Motion-Classifier
=================
written by Marc Zöller <mazoeller@gmail.com>

###1 Introduction

This project aims to create a real time classifier for EMG signals. The idea is it to record the EMG signals, process and evaluate them in real time so that the signals can be assigned to one of nine movements. However, the Motion-Classifier is desinged as a static library. See https://github.com/Ennosigaeon/Motion-Classifier-Console for information how to use the Motion-Classifier. The project is written in C++.


###2 Requirements

To build this project the follwing libraries are required:

#####2.1 Boost
Boost is a collection of libraries, to make the development with C++ easier. It is necessary for for actual project and for the OTBioLabCppClient-develop. See http://www.boost.org/ for more information and downloading instructions. You have to install Boost manually.

#####2.2 OTBioLabCppClient
This code provides a way to read EMG signals from the OT BioLab software. It is available under https://github.com/RehabEngGroup/OTBioLabCppClient. All necessary files are in the folder _libs_, no further installation is needed.

#####2.3 libsvm
This library provides an easy-to-use way to calculate Support Vector Machines. This library is used to do the classification task. See http://www.csie.ntu.edu.tw/~cjlin/libsvm/ for more information. All required files are in the folder _libs_, so no further installation is needed.
	

###3 Usage

To use the classifier you have to provide a configuration file. See Configuration for a detailed description of all properties. There are three important classes you have to deal with:

1. First the EMGProvider. This class defines an abstract interface to read EMG signals. In addition two implementations of the EMGProvider were added: EMGFileProvider and EMGOTProvider. As the name suggests the EMGFileProvider reads EMG signals from a file. The EMGOTProvider reads EMG sensor values from the OT BioLab software. Further informations are in the EMGOTProvider.h file. It is possible to add new ways of reading EMG values by implementing EMGProvider.

2. The second important class is the Classifier. This class offers an abstract way to classify the recorded signals. An example for the implementation of the Classifier is the SVMClassifier. You can add own Classifiers by implementing the Classifier.

3. Often you have to pass trainings data to your Classifier. The Trainer implements some basic trainings procedure, you can add an own trainings process by extending Trainer.

To integrate the Motion-Classifier you have to do something like

	AppConfig::load(path);
    EMGProvider emg;
	
	Classifier classifier;
	//If the classifier needs trainings data
	//Trainer trainer();
	//classifier.train(trainer.train(&emg))
	classifier.send(Signal::START);
	while (1)
		std::cout << classifier.getMuscleMotion() << std::endl;
	classifier.send(Signal::SHUTDOWN);
	
See  https://github.com/Ennosigaeon/Motion-Classifier-Console for a working example.


###4 Configuration

To be able to use the classifier you have to provide a path to a configuration file. This path is passed by the command line parameter "motion_classifier.config" (this parameter may change, see AppConfig.h documentation for the latest parameter).
The configuration file has to have to following format: 

    #this is a comment

    key = value
    key2 = value2
	
Empty lines and lines starting with a '#' will be skipped. The required keys are:

#####1. sample.rows (int)
Defines the number of rows in a single sample.
#####2. sample.columns (int)
Defines the number of columns in a single sample.
#####3. interval.nrSamples (int)
Defines the number of samples a single interval contains.
#####4.	blockingQueue.maxWaitTime (int)
Defines the maximum wait time for the BlockingQueue in milliseconds. The default value is -1. It is recommended to change this value, otherwise you can get a deadlock!
#####5. trainer.baseDir (std::string)
Defines the directory in which all trainings data will be stored.
#####6. trainer.trainingsSize (int)
Defines the size of a trainings window.
#####7. logger.level (int)
Defines the the log level. 0 means 'trace' and 5 means 'fatal'.
#####8. logger.file
Defines a path to a file. All messages will be logged in this file. This property is optional.
	
		
###5 Trouble Shooting

1. 	Unsafe string copy

	Microsoft implemented an own mechanism to check whether std::string copies are correctly done (no buffer overflow,...). The boost library uses other mechanisms to copy std::strings, so the msvc compiler throws a warning. To disable this warning add the flag -D_SCL_SECURE_NO_WARNINGS under Properties -> Configuration -> C/C++ -> Command Line -> additional options (Microsoft Visual Studio 2013).
	A more detailed description is available under [Stackoverlow](http://stackoverflow.com/questions/12270224/xutility2227-warning-c4996-std-copy-impl).
	
2.	Shutdown takes a lot of time

	It takes the tripple time of defined maximal waiting time of the BlockingQueue to shut down the Classifier. I do not know why this is so, but this is only a minor issue. You just have to wait some seconds longer until all threads are finally stoped.
	
3.	Unsafe function or variable

	libsvm-3.18 uses the function some functions that are not in the C++ standard. Therefore MSVC throws the warning 'C4996'. To disable this warning add the following to the beinning of svm.h
	
           #pragma warning(disable : 4996)
	
4.	Training the SVMs creates output

	The svm_train() function of libsvm prints configuration information to the console. To disable/enable this, change the if condition in svm.cpp

           #if 1
           static void info(const char *fmt,...)

5. WIN32_LEAN_AND_MEAN

	Windows.h may throw a compiler error when it is included multiple time and the preprocessor directive is not defined (happens in boost). Therefore add the following to the beginning of your main file:
           #define WIN32_LEAN_AND_MEAN