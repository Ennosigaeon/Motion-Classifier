#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "../h/Exception.h"
#include "../h/SupportVectorMachine.h"

SupportVectorMachine::SupportVectorMachine() {
	config = AppConfig::getInstance();
}

SupportVectorMachine::~SupportVectorMachine() {
	delete[] problem.y;
	delete[] problem.x;
	delete svm;
}

void SupportVectorMachine::addTrainData(const MuscleMotion& motion, std::vector<math::Vector>& vector) {
	if (classA == MuscleMotion::UNKNOWN) {
		classA = motion;
		valuesA.swap(vector);
	}
	else {
		if (classB == MuscleMotion::UNKNOWN) {
			classB = motion;
			valuesB.swap(vector);
		}
	}
}

void SupportVectorMachine::calculateSVM() {
	if (classA == MuscleMotion::UNKNOWN || classB == MuscleMotion::UNKNOWN || valuesA.empty() || valuesB.empty())
		throw Exception::SVM_MISSING_TRAININGS_DATA;
	BOOST_LOG_TRIVIAL(info) << "Training SVM for MuscleMotions " << classA << " (" << valuesA.size() << " values) and "
		<< classB << " (" << valuesA.size() << " values).";

	//converts the two given std::vectors in a svm_problem
	std::vector<math::Vector> list(valuesA);
	list.insert(list.end(), valuesB.begin(), valuesB.end());

	problem.l = list.size();
	double *label = new double[problem.l];
	for (unsigned int i = 0; i < valuesA.size(); i++)
		label[i] = -1;
	for (unsigned int i = valuesA.size(); i < problem.l; i++)
		label[i] = 1;
	problem.y = label;

	svm_node **nodes = new svm_node*[problem.l];
	int i = 0;
	for (std::vector<math::Vector>::iterator it = list.begin(); it != list.end(); it++, i++)
		nodes[i] = createNode(&(*it));
	problem.x = nodes;

	//creates the svm_parameters
	svm_parameter *param = config->getSVMParameter();
	svm_check_parameter(&problem, param);

	//creates the model for the svm
	svm = svm_train(&problem, param);
	BOOST_LOG_TRIVIAL(trace) << "Trained SVM for " << classA << ", " << classB;
}

MuscleMotion SupportVectorMachine::classify(std::vector<math::Vector>& vector) {
	int matchA = 0;
	int matchB = 0;

	for (std::vector<math::Vector>::iterator it = vector.begin(); it != vector.end(); it++) {
		svm_node *node = createNode(&(*it));
		double result = svm_predict(svm, node);
		result == -1 ? matchA++ : matchB++;
		delete[] node;
	}

	if (matchA > matchB) {
		BOOST_LOG_TRIVIAL(trace) << "variogram assigned to " << classA;
		return classA;
	}
	if (matchB > matchA) {
		BOOST_LOG_TRIVIAL(trace) << "variogram assigned to " << classB;
		return classB;
	}
	BOOST_LOG_TRIVIAL(trace) << "unable to assign variogram";
	return MuscleMotion::UNKNOWN;
}


svm_node* SupportVectorMachine::createNode(math::Vector* vector) {
	svm_node *node = new svm_node[3];
	node[0].index = 1;
	node[0].value = vector->getX();
	node[1].index = 2;
	node[1].value = vector->getY();
	node[2].index = -1;
	node[2].value = 0;

	return node;
}