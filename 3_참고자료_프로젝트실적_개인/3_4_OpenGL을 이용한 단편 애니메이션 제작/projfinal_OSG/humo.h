#ifndef HUMO_H
#define HUMO_H

#include "diging.h"
#include <vector>

// modeling 변수
#define head_HEIGHT 1.0
#define torso_HEIGHT 2.0
#define upper_arm_HEIGHT 1.0
#define lower_arm_HEIGHT 1.0
#define upper_leg_HEIGHT 1.0
#define lower_leg_HEIGHT 1.0


//hiearchicaltransfomation에 하는 변수
#define head_rot_x 0
#define head_rot_y 1
#define neck_rot_x 2

#define left_upper_arm_rot_x 3
#define left_upper_arm_rot_y 4
#define left_upper_arm_rot_z 5
#define left_lower_arm_rot_x 6

#define right_upper_arm_rot_x 7
#define right_upper_arm_rot_y 8
#define right_upper_arm_rot_z 9
#define right_lower_arm_rot_x 10

#define left_upper_leg_rot_x 11
#define left_upper_leg_rot_y 12
#define left_lower_leg_rot_x 13
#define left_foot_rot_x 14

#define right_upper_leg_rot_x 15
#define right_upper_leg_rot_y 16
#define right_lower_leg_rot_x 17
#define right_foot_rot_x 18

#define torso_pos_y 19
// view  변수

class humo
{
public : 
	humo();
	void render();
	mat4 autorun(float);
	vec4 position;
	vec3 destination;
	float interpspeed;
	bool d_isset = false;
	void reset();
};

#endif HUMO_H