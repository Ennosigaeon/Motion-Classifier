#ifndef DIRPROVIDER_H
#define DIRPROVIDER_H

#include <map>
#include <random>
#include "EMGProviderImpl.h"
#include "Logger.h"
#include "Motion.h"

namespace motion_classifier {

	class DirProvider : public EMGProviderImpl {
	private:
		Logger *logger = Logger::getInstance();
		std::string folder;
		double electrodeLost = 0;
		bool transversalShift = false;
		bool longitudinalShift = false;


		std::map<Motion::Muscle, std::vector<std::vector<Interval*>*>*> intervals;
		Motion::Muscle lastMotion = Motion::Muscle::UNKNOWN;

		std::default_random_engine generator;
		std::uniform_int_distribution<int> pairsRnd{ 1, 9 };
		std::uniform_int_distribution<int> *listRnd = NULL;
		std::uniform_int_distribution<int> *intervalRnd = NULL;

	public:
		DirProvider(std::string folder, double electrodeLost = 0, bool longitudinalShift = false, bool transversalShift = false);

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
		  * Loads all Intervals from the specified folder. Parameters like electrode
		  * lost or shift are applied to all loaded Intervals. All Intervals for a
		  * movement have to be stored in a file named as specified in Utilities::
		  * getMotion(). Otherwise the Intervals are assigned to the movement UNKNOWN.
		  * The expected file format is "<DIR>/<MOTION>-<1,...,count>.txt"
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
		  * Returns the status of transversal shift.
		  */
		bool getTransversalShift() const;

		/**
		  * Returns the status of longitudinal shift.
		  */
		bool getLongitudinalShift() const;

		/**
		  * Sets the electrode lost. Use loadIntervals() to	apply the change.
		  */
		void setElectrodeLost(double lost);

		/**
		  * Sets the longitudinal shift. Use loadIntervals() to	apply the change.
		  */
		void setLongitudinalShift(bool shift);

		/**
		  * Sets the transversal shift. Use loadIntervals() to	apply the change.
		  */
		void setTransversalShift(bool shift);
	};

}
#endif