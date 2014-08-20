#include <boost/lexical_cast.hpp>
#include <fstream>
#include "../h/DirProvider.h"
#include "../h/Utilities.h"

using namespace motion_classifier;

DirProvider::DirProvider(std::string folder, double electrodeLost, bool longitudinalShift, bool transversalShift) :
folder(folder), electrodeLost(electrodeLost), longitudinalShift(longitudinalShift), transversalShift(transversalShift) {
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
	auto *result = new std::map < Motion::Muscle, std::vector<Interval*>* > ;

	for (const auto &pair : intervals) {
		auto list = new std::vector < Interval* > ;
		for (const auto &l : *pair.second) {
			auto it = l->begin();
			it = std::next(it, start);
			for (int i = start; i < end; ++i) {
				Interval *interval = new Interval;
				interval->addSample(new Sample(*(**it).getMeanSample()));
				list->push_back(interval);
				++it;
			}
		}
		result->insert(std::make_pair(pair.first, list));
	}

	return result;
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
	for (const auto &pair : intervals) {
		for (const auto &interval : *pair.second)
			delete interval;
		delete pair.second;
	}
	intervals.clear();

	for (int i = Motion::Muscle::REST_POSITION; i <= Motion::Muscle::HAND_CLOSE; ++i) {
		Motion::Muscle movement = static_cast<Motion::Muscle>(i);
		auto *list = new std::vector<std::vector<Interval*>*>;
		for (int i = 1; i < count; ++i) {
			std::vector<Interval*> *list2 = new std::vector<Interval*>;

			std::ifstream in(folder + printMotion(movement) + "-" + boost::lexical_cast<std::string>(i)+".txt");
			while (!in.eof()) {
				Interval *interval = new Interval;
				Sample *s = new Sample;
				try {
					in >> *s;

					//TODO: handle electrode lost and shift here

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

bool DirProvider::getTransversalShift() const {
	return transversalShift;
}

bool DirProvider::getLongitudinalShift() const {
	return longitudinalShift;
}

void DirProvider::setElectrodeLost(double lost) {
	DirProvider::electrodeLost = lost;
}

void DirProvider::setLongitudinalShift(bool shift) {
	DirProvider::longitudinalShift = shift;
}

void DirProvider::setTransversalShift(bool shift) {
	DirProvider::transversalShift = shift;
}