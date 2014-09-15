#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/DirProvider.h"
#include "../h/Utilities.h"

using namespace motion_classifier;

DirProvider::DirProvider(std::string folder, double electrodeLost, Shift shift) :
folder(folder), electrodeLost(electrodeLost), shift(shift) {
}

DirProvider::~DirProvider() {
	for (const auto &pair : intervals) {
		for (const auto &list : *pair.second) {
			for (const auto &interval : *list)
				delete interval;
			delete list;
		}
		delete pair.second;
	}
	delete listRnd;
	delete intervalRnd;
}

Interval* DirProvider::getInterval() {
	if (intervals.empty())
		return NULL;

	lastMotion = static_cast<Motion::Muscle>(pairsRnd(generator));
	return intervals.at(lastMotion)->at((*listRnd)(generator))->at((*intervalRnd)(generator));
}

Motion::Muscle DirProvider::getMotionForLastInterval() const{
	return lastMotion;
}

std::map<Motion::Muscle, std::vector<Interval*>*>* DirProvider::getIntervalSubset(int start, int end) {
	logger->debug("Creating interval subset");
	auto *result = new std::map < Motion::Muscle, std::vector<Interval*>* > ;

	//init electrode lost
	AppConfig *config = AppConfig::getInstance();
	int sampleSize = config->getSampleColumns() * config->getSampleRows();
	std::uniform_int_distribution<int> lostRnd(0, sampleSize - 1);
	std::vector<int> lost;
	for (int i = 0; i < (int)round(electrodeLost * sampleSize); ++i) {
		int index;
		do {
			index = lostRnd(generator);
			auto it = std::find(lost.begin(), lost.end(), index);
			if (it == lost.end())
				break;
		} while (true);
		lost.push_back(index);
	}

	for (const auto &pair : intervals) {
		auto list = new std::vector < Interval* > ;
		for (const auto &l : *pair.second) {
			auto it = l->begin();
			if (start != 0)
				it = std::next(it, start - 1);
			for (int i = start; i < end; ++i) {
				Sample *s = new Sample(*(**it).getMeanSample());

				//apply electrode lost
				for (const auto &i : lost)
					s->getEntries()[i].set(math::Dimension::Z, NAN);

				applyShift1(s);

				Interval *interval = new Interval;
				interval->addSample(s);
				interval->setGroup((**it).getGroup());
				list->push_back(interval);
				++it;
			}
		}
		result->insert(std::make_pair(pair.first, list));
	}

	return result;
}

void DirProvider::applyShift1(Sample *s) {
	switch (shift) {
	case LONGITUDINAL_DOWN:
		//first, thrid, ... row
		for (int x = 0; x < s->getNrColumns(); ++x) {
			for (int y = 1; y < s->getNrRows(); y += 2)
				s->getEntries()[x * s->getNrRows() + y].set(math::Dimension::Z, NAN);
		}
		break;
	case LONGITUDINAL_UP:
		//second, fourth ... row
		if (shift == LONGITUDINAL_UP)
			for (int x = 0; x < s->getNrColumns(); ++x)
				for (int y = 0; y < s->getNrRows(); y += 2)
					s->getEntries()[x * s->getNrRows() + y].set(math::Dimension::Z, NAN);
		break;
	case TRANSVERSAL_INWARDS:
		//frist, third, ... column
		for (int x = 1; x < s->getNrColumns(); x += 2)
			for (int y = 0; y < s->getNrRows(); ++y)
				s->getEntries()[x * s->getNrRows() + y].set(math::Dimension::Z, NAN);
		break;
	case TRANSVERSAL_OUTWARDS:
		//second, fourth, ... column
		for (int x = 0; x < s->getNrColumns(); x += 2)
			for (int y = 0; y < s->getNrRows(); ++y)
				s->getEntries()[x * s->getNrRows() + y].set(math::Dimension::Z, NAN);
	}
}

void DirProvider::applyShift2(Sample *s) {
	switch (shift) {
	case LONGITUDINAL_DOWN:
		//first row removed
		for (int x = 0; x < s->getNrColumns(); ++x) {
			for (int y = 0; y < s->getNrRows(); ++y) {
				math::Vector *vec = &(s->getEntries()[x * s->getNrRows() + y]);
				if (y == 0)
					vec->set(math::Dimension::Z, NAN);
				vec->set(math::Dimension::Y, vec->get(math::Dimension::Y) - 1);
			}
		}
		break;
	case LONGITUDINAL_UP:
		//last row removed
		for (int x = 0; x < s->getNrColumns(); ++x) {
			for (int y = 0; y < s->getNrRows(); ++y) {
				math::Vector *vec = &(s->getEntries()[x * s->getNrRows() + y]);
				if (y == s->getNrRows() - 1)
					vec->set(math::Dimension::Z, NAN);
				vec->set(math::Dimension::Y, vec->get(math::Dimension::Y) + 1);
			}
		}
		break;
	case TRANSVERSAL_INWARDS:
		//last column removed
		for (int x = 0; x < s->getNrColumns(); ++x) {
			for (int y = 0; y < s->getNrRows(); ++y) {
				math::Vector *vec = &(s->getEntries()[x * s->getNrRows() + y]);
				if (x == 0)
					vec->set(math::Dimension::Z, NAN);
				vec->set(math::Dimension::X, vec->get(math::Dimension::X) - 1);
			}
		}
		break;
	case TRANSVERSAL_OUTWARDS:
		//first column removed
		for (int x = 0; x < s->getNrColumns(); ++x) {
			for (int y = 0; y < s->getNrRows(); ++y) {
				math::Vector *vec = &(s->getEntries()[x * s->getNrRows() + y]);
				if (x == s->getNrColumns() - 1)
					vec->set(math::Dimension::Z, NAN);
				vec->set(math::Dimension::X, vec->get(math::Dimension::X) + 1);
			}
		}
		break;
	}
}

void DirProvider::releaseIntervalSubset(std::map<Motion::Muscle, std::vector<Interval*>*>* data) {
	for (const auto &pair : *data) {
		for (const auto &interval : *pair.second)
			delete interval;
		delete pair.second;
	}
	delete data;
}

void DirProvider::loadIntervals(int count) {
	logger->debug("Loading intervals");
	for (const auto &pair : intervals) {
		for (const auto &interval : *pair.second)
			delete interval;
		delete pair.second;
	}
	intervals.clear();

	for (int i = Motion::Muscle::REST_POSITION; i <= Motion::Muscle::HAND_CLOSED; ++i) {
		Motion::Muscle movement = static_cast<Motion::Muscle>(i);
		auto *list = new std::vector<std::vector<Interval*>*>;
		for (int i = 1; i < count; ++i) {
			std::vector<Interval*> *list2 = new std::vector<Interval*>;

			std::ifstream in(folder + printMotion(movement) + "-" + boost::lexical_cast<std::string>(i)+".txt");
			while (!in.eof()) {
				Interval *interval = new Interval;
				interval->setGroup(i);
				Sample *s = new Sample;
				try {
					in >> *s;
					interval->addSample(s);
					list2->push_back(interval);
				}
				catch (const std::out_of_range &ex) {
					//Ignore.
				}
			}
			list->push_back(list2);
		}
		intervals.insert(std::make_pair(movement, list));
	}
	if (!intervals.empty()) {
		delete intervalRnd;
		delete listRnd;

		intervalRnd = new std::uniform_int_distribution<int>(0, (*intervals.begin()->second->begin())->size() - 1);
		listRnd = new std::uniform_int_distribution<int>(0, count - 2);
	}
}

void DirProvider::send(const Signal &signal) {
	//Not needed.
}

double DirProvider::getElectrodeLost() const {
	return electrodeLost;
}

Shift DirProvider::getShift() const {
	return shift;
}

void DirProvider::setElectrodeLost(double lost) {
	DirProvider::electrodeLost = lost;
}

void DirProvider::setShift(const Shift shift) {
	DirProvider::shift = shift;
}
