#ifndef RUNNINGDOG_H
#define RUNNINGDOG_H

#include "diging.h"
#include <vector>

#define head_HEIGHT 1.0
#define torso_HEIGHT 1.0
#define upper_arm_HEIGHT 1.0
#define lower_arm_HEIGHT 1.0
#define upper_leg_HEIGHT 1.0
#define lower_leg_HEIGHT 1.0

#define head_rot_x 0
#define head_rot_y 1
#define head_rot_z 2

#define torso_trans_x 3
#define torso_trans_z 4
#define torso_rot_x 5
#define torso_rot_y 6

#define front_left_leg 7
#define front_right_leg 8
#define back_left_leg 9
#define back_right_leg 10

#define tongue_rot_z 11
#define tail_rot_x 12

#define default_y 1.0

class runningDog{
public:
	runningDog();
	void render();
	mat4 autorun(float);
	vec4 position;
	vec3 destination;
	float interpspeed;
	void reset();
};

#endif RUNNINGDOG_H