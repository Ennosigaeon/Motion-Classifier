#ifndef DIRPROVIDER_H
#define DIRPROVIDER_H

#include <map>
#include <random>
#include "EMGProviderImpl.h"
#include "Logger.h"
#include "Motion.h"

namespace motion_classifier {

	enum Shift {
		//Uses all electrodes
		NO,

		//Uses the second, fourth, ... row from bottom contains values
		LONGITUDINAL_UP,

		//Uses the first, thrid, ... row from bottom contains values
		LONGITUDINAL_DOWN,

		//Defined for the right hand. Uses first, third, ... column contains values
		TRANSVERSAL_INWARDS,

		//Defined for the right hand. Uses second, fourth, ... column contains values
		TRANSVERSAL_OUTWARDS
	};

	class DirProvider : public EMGProviderImpl {
	private:
		Logger *logger = Logger::getInstance();
		std::string folder;
		double electrodeLost = 0;
		Shift shift = Shift::NO;


		std::map<Motion::Muscle, std::vector<std::vector<Interval*>*>*> intervals;
		Motion::Muscle lastMotion = Motion::Muscle::UNKNOWN;

		std::default_random_engine generator;
		std::uniform_int_distribution<int> pairsRnd{ 1, 9 };
		std::uniform_int_distribution<int> *listRnd = NULL;
		std::uniform_int_distribution<int> *intervalRnd = NULL;

	public:
		DirProvider(std::string folder, double electrodeLost = 0, Shift shift = Shift::NO);

		~DirProvider();

		/**
		  * Returns a random Interval.
		  */
		virtual Interval* getInterval();

		/**
		  * Returns the according movement for the last Interval
		  * returned by getInterval().
		  */
		Motion::Muscle getMotionForLastInterval() const;

		/**
		  * Returns a subset of all Intervals. Start is included, end is excluded.
		  */
		std::map<Motion::Muscle, std::vector<Interval*>*>* getIntervalSubset(int start, int end);

		/**
		  * Deletes an Interval subset created with getIntervalSubset(int, int).
		  */
		void releaseIntervalSubset(std::map<Motion::Muscle, std::vector<Interval*>*>* data);

		/**
		  * Loads all Intervals from the specified folder. All Intervals for a
		  * movement have to be stored in a file named as specified in Utilities::
		  * getMotion(). Otherwise the Intervals are assigned to the movement
		  * UNKNOWN. The expected file format is "<DIR>/<MOTION>-<1,...,count>.txt"
		  */
		void loadIntervals(int count);

		/**
		  * This function is not needed, but has to implemented
		  */
		virtual void send(const Signal &signal);

		/**
		  * Returns the current electrode lost.
		  */
		double getElectrodeLost() const;

		/**
		  * Returns the status of shift.
		  */
		Shift getShift() const;

		/**
		  * Sets the electrode lost.
		  */
		void setElectrodeLost(double lost);


		/**
		  * Sets the shift.
		  */
		void setShift(const Shift shift);
	};

}
#endif