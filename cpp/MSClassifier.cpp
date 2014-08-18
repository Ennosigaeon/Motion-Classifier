
#include <array>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/MSClassifier.h"
#include "../h/Utilities.h"

using namespace motion_classifier;

MSClassifier::MSClassifier(EMGProvider *emg, Properties *configuration) {
	MSClassifier::emgProvider = emg;
	h = configuration->getDouble("ms.h");
	m = configuration->getInt("matrix.m");
	n = configuration->getInt("matrix.n");
	p = configuration->getInt("matrix.p");

	double minX = configuration->getDouble("space.x.min"), maxX = configuration->getDouble("space.x.max"),
		minY = configuration->getDouble("space.y.min"), maxY = configuration->getDouble("space.y.max"),
		minZ = configuration->getDouble("space.z.min"), maxZ = configuration->getDouble("space.z.max");
	if (isnan(minX))
		minX = -std::numeric_limits<double>::infinity();
	if (isnan(maxX))
		maxX = std::numeric_limits<double>::infinity();
	if (isnan(minY))
		minY = -std::numeric_limits<double>::infinity();
	if (isnan(maxY))
		maxY = std::numeric_limits<double>::infinity();
	if (isnan(minZ))
		minY = -std::numeric_limits<double>::infinity();
	if (isnan(maxZ))
		maxY = std::numeric_limits<double>::infinity();
	math::Space *space = new math::Space(minX, maxX, minY, maxY, minZ, maxZ);
	math::Vector::setSpace(space);

	msAlgo = new MeanShift(static_cast<math::KernelType>(configuration->getInt("ms.kernel")),
		configuration->getDouble("ms.epsilon"), configuration->getDouble("ms.threshold"));
	msAlgo->setFiltering(configuration->getBool("ms.filtering"));
}

MSClassifier::~MSClassifier() {
	for (auto &pair : trainingsData)
		delete pair.second;
	for (auto &pair : trainingsVectors) {
		for (auto &vector : *pair.second)
			delete vector;
		delete pair.second;
	}
	delete msAlgo;
	math::Vector::setSpace(new math::Space);
}


void MSClassifier::train(std::string folder) {
	auto training = extractTrainingsData(folder);

	logger->info("starting trainings procedure");
	for (auto &pair : *training) {
		std::vector<math::Vector*> allCenters;
		for (Interval *interval : *pair.second) {
			Sample *mean = interval->getMeanSample();
			msAlgo->setDataPoints(mean->getEntries(), mean->getSize());
			std::vector<math::Vector*> *centers = msAlgo->calculate(h);
			allCenters.insert(allCenters.end(), centers->begin(), centers->end());
		}

		math::Vector *vectors = new math::Vector[allCenters.size()];
		int i = 0;
		for (auto &vec : allCenters)
			vectors[i++] = *vec;
		msAlgo->setDataPoints(vectors, allCenters.size());
		trainingsVectors.insert(std::make_pair(pair.first, msAlgo->calculate(h)));

		delete[] vectors;
	}
	msAlgo->setDataPoints(NULL, 0);
	calcTrainingsMatrix();

//	for (const auto &pair : trainingsVectors) {
//		std::ofstream out("C:/Tmp/MS\ Centers/" + printMotion(pair.first) + ".txt");
//		for (const auto & vector : *pair.second)
//			out << *vector << std::endl;
//		out.close();
//	}

	for (auto &pair : *training) {
		for (auto &interval : *pair.second)
			delete interval;
		delete pair.second;
	}
	delete training;
}

Motion::Muscle MSClassifier::classify(Interval *interval) {
	logger->trace("calculating mean sample");
	Sample *mean = interval->getMeanSample();

	logger->trace("calculating cluster centers");
	msAlgo->setDataPoints(mean->getEntries(), mean->getSize());
	auto centers = msAlgo->calculate(h);

	logger->trace("matching clusters");
	std::vector<std::pair<Motion::Muscle, double>> distances;

	math::Matrix ref(m, n, p);
	for (const auto &vec : *centers)
		ref.assignToBucket(*vec);
	ref.normalize();

	for (const auto &pair : trainingsData)
		distances.push_back(std::make_pair(pair.first, ref.getDistance(*pair.second)));

	if (distances.empty())
		return Motion::Muscle::UNKNOWN;

	std::sort(distances.begin(), distances.end(), [](const std::pair<Motion::Muscle, double> &a, const std::pair<Motion::Muscle, double> &b) {
		return a.second < b.second;
	});

	logger->trace("minimum distance: " + boost::lexical_cast<std::string>(distances.at(0).second));
	return distances.at(0).first;
}

//This code has to be in a seperate function so it can be invoked from CrossCorrelation.
void MSClassifier::calcTrainingsMatrix() {
	for (const auto &pair : trainingsData)
		delete pair.second;
	trainingsData.clear();

	for (const auto &pair : trainingsVectors) {
		math::Matrix *mat = new math::Matrix(m, n, p);
		for (const auto &vec : *pair.second)
			mat->assignToBucket(*vec);
		mat->normalize();
		trainingsData.insert(std::make_pair(pair.first, mat));
	}
}

//This function is only used to extract data from the HDD and convert them into the right format. HAS TO BE REMOVED!!!
std::map< Motion::Muscle, std::vector<Interval*>*>* MSClassifier::extractTrainingsData(std::string folder) {
	logger->info("loading trainings data for MSClassifier");
	auto data = new std::map<Motion::Muscle, std::vector<Interval*>*>;

	std::array<std::string, 9> movements = { "pronation", "supination", "extension", "flexion", "open", "close", "rest", "ulnar", "radial" };
	for (auto &movement : movements) {
		std::vector<Interval*> *list = new std::vector < Interval* >;
		for (int i = 1; i < 5; ++i) {
			std::ifstream in(folder + movement + "-" + boost::lexical_cast<std::string>(i)+".txt");
			//Skip intervals
			for (int j = 0; j < 14 - NR_INTERVALS; ++j) {
				std::string s;
				std::getline(in, s);
			}
			for (int j = 0; j < NR_INTERVALS * 2 + 1; ++j) {
				Interval *interval = new Interval;
				Sample *s = new Sample;
				in >> *s;
				interval->addSample(s);
				list->push_back(interval);
			}
		}
		data->insert(std::make_pair(motion_classifier::getMotion(movement), list));
	}
	return data;
}
