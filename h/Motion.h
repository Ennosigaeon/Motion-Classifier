#ifndef MUSCLE_MOTION_H
#define MUSCLE_MOTION_H

/**
  * This namespace contains enums with possible Motions. At the moment
  * only the motion of a muscle is defined, but it is possible to add
  * other motion types (like accelerometer or GPS).
  */
namespace Motion {
	/**
	  * This enum defines all possible motions of a Muscle.
	  */
	enum Muscle {
		//Not possible to determine muscle motion
		UNKNOWN,

		//arm is in the rest position, no active motion
		REST_POSITION,

		//Subject rolls hand down in direction of the innerside of the forarm
		WRIST_FLEXION,

		//Subject pulls hand up in direction of the outerside of the forarm
		WRIST_EXTENSION,

		//Subject bends hand outwards, parallel to palm
		RADIAL_DEVIATION,

		//Subject bends hand inwards, parallel to palm
		ULNAR_DEVIATION,

		//Subject turns the forarm inwards
		FOREARM_PRONATION,

		//Subject turns the forarm outwards
		FOREARM_SUPINATION,

		//Subject is opening its hand
		HAND_OPEN,

		//Subject is closing its hand
		//Note: It is important, that this element is always the last one! Index is used several times.
		HAND_CLOSE
	};
}

#endif