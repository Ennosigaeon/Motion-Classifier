#include <boost/lexical_cast.hpp>
#include "../h/Logger.h"
#include "../h/SupportVectorMachine.h"
#include "../h/Utilities.h"

using namespace motion_classifier;

SupportVectorMachine::SupportVectorMachine(svm_parameter *param) {
	SupportVectorMachine::param = param;
}

SupportVectorMachine::~SupportVectorMachine() {
	delete[] problem.y;
	delete[] problem.x;
	delete svm;
}

void SupportVectorMachine::addTrainData(const Motion::Muscle& motion, std::vector<math::Vector> *vector) {
	if (classA == Motion::Muscle::UNKNOWN) {
		classA = motion;
		valuesA.insert(valuesA.end(), vector->begin(), vector->end());
	}
	else {
		if (classB == Motion::Muscle::UNKNOWN) {
			classB = motion;
			valuesB.insert(valuesB.end(), vector->begin(), vector->end());
		}
	}
}

void SupportVectorMachine::calculateSVM() {
	if (classA == Motion::Muscle::UNKNOWN || classB == Motion::Muscle::UNKNOWN || valuesA.empty() || valuesB.empty())
		throw std::domain_error("no data for at least one class");
	Logger::getInstance()->debug("Training SVM for MuscleMotions " + motion_classifier::printMotion(classA) + " (" + boost::lexical_cast<std::string>(valuesA.size()) + " values) and "
		+ motion_classifier::printMotion(classB) + " (" + boost::lexical_cast<std::string>(valuesA.size()) + " values).");

	//converts the two given std::vectors in a svm_problem
	std::vector<math::Vector> list(valuesA);
	list.insert(list.end(), valuesB.begin(), valuesB.end());

	problem.l = list.size();
	double *label = new double[problem.l];
	for (unsigned int i = 0; i < valuesA.size(); ++i)
		label[i] = -1;
	for (int i = valuesA.size(); i < problem.l; ++i)
		label[i] = 1;
	problem.y = label;

	problem.x = new svm_node*[problem.l];
	int i = 0;
	for (math::Vector &vector : list)
		problem.x[i++] = createNode(&vector);

	//creates the svm_parameters
	svm_check_parameter(&problem, param);

	//creates the model for the svm
	svm = svm_train(&problem, param);
	Logger::getInstance()->trace("Trained SVM for " + printMotion(classA) + " & " + printMotion(classB));
}

Motion::Muscle SupportVectorMachine::classify(std::vector<math::Vector> *vector) {
	int matchA = 0;
	int matchB = 0;

	for (auto &vec : *vector) {
		svm_node *node = createNode(&vec);
		double result = svm_predict(svm, node);
		result == -1 ? ++matchA : ++matchB;
		delete[] node;
	}

	if (matchA > matchB) {
		Logger::getInstance()->trace("variogram assigned to " + motion_classifier::printMotion(classA));
		return classA;
	}
	if (matchB > matchA) {
		Logger::getInstance()->trace("variogram assigned to " + motion_classifier::printMotion(classB));
		return classB;
	}
	Logger::getInstance()->trace("unable to assign variogram");
	return Motion::Muscle::UNKNOWN;
}


svm_node* SupportVectorMachine::createNode(math::Vector* vector) {
	svm_node *node = new svm_node[3];
	node[0].index = 1;
	node[0].value = vector->getLength(2);
	node[1].index = 2;
	node[1].value = vector->get(math::Dimension::Z);
	node[2].index = -1;
	node[2].value = 0;

	return node;
}