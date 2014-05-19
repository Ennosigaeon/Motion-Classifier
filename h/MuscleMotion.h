
#ifndef MUSCLE_MOTION_H
#define MUSCLE_MOTION_H

/**
  * This enum defines all possible motions of the arm. The result
  * of the classification will be an MuscleMotion.
  */
enum MuscleMotion {

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
	FORARM_PRONATION,

	//Subject turns the forarm outwards
	FORARM_SUPINATION,

	//Subject is opening its hand
	HAND_OPEN,

	//Subject is closing its hand
	HAND_CLOSE
};

#endif