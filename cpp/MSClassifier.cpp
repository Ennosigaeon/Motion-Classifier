#include "../h/Logger.h"
#include "../h/MSClassifier.h"

#include <array>
#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/Utilities.h"

#define NR_INTERVALS 1

using namespace motion_classifier;

MSClassifier::MSClassifier(EMGProvider *emg, Properties *configuration) {
	MSClassifier::emgProvider = emg;
	logger = Logger::getInstance();
	h = configuration->getDouble("ms.h");

	double minX = configuration->getDouble("space.x.min"), maxX = configuration->getDouble("space.x.max"), minY = configuration->getDouble("space.y.min"),
		maxY = configuration->getDouble("space.x.max");
	if (isnan(minX))
		minX = -std::numeric_limits<double>::infinity();
	if (isnan(maxX))
		maxX = std::numeric_limits<double>::infinity();
	if (isnan(minY))
		minY = -std::numeric_limits<double>::infinity();
	if (isnan(maxY))
		maxY = std::numeric_limits<double>::infinity();
	math::Space *space = new math::Space(minX, maxX, minY, maxY);
	space->setNorm(static_cast<math::Norm>(configuration->getInt("space.norm")));
	
	msAlgo = new MeanShift(static_cast<math::KernelType>(configuration->getInt("ms.kernel")),
		space, configuration->getDouble("ms.epsilon"), configuration->getDouble("ms.threshold"));
	msAlgo->setFiltering(configuration->getBool("ms.filtering"));
}

MSClassifier::~MSClassifier() {
	for (auto pair : trainingsData) {
		for (auto vector : *pair.second)
			delete vector;
		delete pair.second;
	}
	delete msAlgo;
	math::Vector::setSpace(new math::Space);
}

void MSClassifier::send(const Signal& signal) {
	if (signal == Signal::START) {
		if (status == Status::NEW) {
			//start EMGProvider
			emgProvider->send(Signal::START);

			//start worker thread
			status = Status::RUNNING;
			worker = std::thread(&MSClassifier::run, this);
		}
		else {
			status = Status::RUNNING;
			std::unique_lock<std::mutex> mlock(mutex);
			mlock.unlock();
			condition.notify_one();
		}
	}
	if (signal == Signal::STOP) {
		status = Status::WAITING;
		emgProvider->send(Signal::STOP);
	}
	if (signal == Signal::SHUTDOWN) {
		status = Status::FINISHED;
		//release waiting thread
		std::unique_lock<std::mutex> mlock(mutex);
		mlock.unlock();
		condition.notify_one();

		//wait for worker to stop
		if (worker.joinable())
			worker.join();

		//stop the EMGProvider
		emgProvider->send(Signal::SHUTDOWN);
	}
}

void MSClassifier::run() {
	Logger *logger = Logger::getInstance();
	while (true) {
		if (status == Status::RUNNING) {
			logger->debug("waiting for new Interval");
			Interval *interval = emgProvider->getInterval();
			if (interval == NULL)
				continue;

			clock_t t = clock();
			logger->debug("calculating mean sample");
			Sample *mean = interval->getMeanSample();

			logger->debug("calculating cluster centers");
			msAlgo->setDataPoints(mean->getEntries(), mean->getSize());
			std::vector<math::Vector*> *centers = msAlgo->calculate(h);

			//TODO: Classify vectors

			for (auto &vec : *centers)
				delete vec;
			delete centers;
			delete interval;
		}
		if (status == Status::WAITING) {
			logger->debug("Classifier stops processing Intervals");
			std::unique_lock<std::mutex> lk(mutex);
			condition.wait(lk);
		}
		if (status == Status::FINISHED) {
			logger->info("shutting down MSClassifier worker");
			return;
		}
	}
}

void MSClassifier::train(std::map<Motion::Muscle, std::vector<Interval*>*>* training) {
	logger->info("starting trainings procedure");
	for (auto &pair : *training) {
		std::vector<math::Vector*> *allCenters = new std::vector < math::Vector* > ;
		for (Interval *interval : *pair.second) {
			Sample *mean = interval->getMeanSample();
			msAlgo->setDataPoints(mean->getEntries(), mean->getSize());
			std::vector<math::Vector*> *centers = msAlgo->calculate(h);
			allCenters->insert(allCenters->end(), centers->begin(), centers->end());
		}

		//TODO: center pruning is not good, too much centers left
		auto it = allCenters->begin();
		while (it != allCenters->end()) {
			if ((**it).getGroup() == -1) {
				it = allCenters->erase(it);
				continue;
			}

			auto it2 = it;
			it2++;
			for (; it2 != allCenters->end(); ++it2) {
				if ((**it).getDistance(**it2) < h) {
					**it += **it2;
					**it /= 2;
					(**it2).setGroup(-1);
				}
			}
			it++;
		}

		trainingsData.insert(std::make_pair(pair.first, allCenters));
	}
	msAlgo->setDataPoints(NULL, 0);
}

std::map<Motion::Muscle, std::vector<math::Vector*>*>* MSClassifier::getTrainingsData() {
	return &trainingsData;
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
