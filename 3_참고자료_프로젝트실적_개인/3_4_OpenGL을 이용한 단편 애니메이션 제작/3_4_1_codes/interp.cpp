#include "interp.h"


inline float randFloat(int k) { return ((float)rand() / (RAND_MAX / (k+1))); }


inline mat4 RotateYfromCS(float cost, float sint)
{
	mat4 c(1.0);
	c[2][2] = c[0][0] = cost;
	c[0][2] = sint;
	c[2][0] = -c[0][2];
	return c;
}

vec3 rand_destination(float rad){
	vec3 R;
	float theta = M_PI * randFloat(rand());
	R.x = rad*sin(theta);
	R.z = rad*cos(theta);

	return R;
}

mat4 get_speed_angle(vec4 p0, vec3 destination, float& speed,float& cosV, float& sinV){


	vec3 p3 = vec3(p0.x, 0, p0.z);
	vec3 diffvecter = destination-p3;

	float cosVal = dot(diffvecter, p3);
	cosVal /= length(diffvecter)*length(p3);

	float sinVal = sqrtf(1.0 - pow(cosVal, 2.0));
	
	mat4 rotangle = RotateYfromCS(cosVal, sinVal);
	printf("%f\n", cosVal);
	printf("%f\n", sinVal);


	vec4 k = mvmult(rotangle, vec4(1.0));
	speed = length(diffvecter) / 100.0f;
	if (speed > 100)
		speed = 1.0;
	cosV = cosVal;
	sinV = sinVal;
	return rotangle;
}