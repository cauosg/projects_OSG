#include "diging.h"
#include "vbocube.h"
#include <vector>
#include "runningDog.h"
#include "interp.h"
#include "humo.h"

extern humo *me;
extern GLint mv_location;
extern mat4 view;
extern mat4 model;
extern GLuint rendering_program;
extern bool autoR;
mat4 dog_rotM(1.0);
mat4 dog_trM(1.0);

VBOCube *dog_cube;
vector <mat4> models;

float dog_sets[14][100];
float c_dog_sets[14];

void runningDog::reset(){


}


mat4 runningDog::autorun(float speed){
	static float cosV, sinV;
	static float distz = 0.0;
	static float frame = 100;
	static float time = 0.0f;
	static mat4 ctmI(1.0);

	for (int i = 0; i < 4; i++)
	{
		c_dog_sets[i + 7] += speed*15;
	}
	c_dog_sets[head_rot_x] = speed*cos(c_dog_sets[front_right_leg] / 50);
	c_dog_sets[head_rot_z] = speed*c_dog_sets[head_rot_x];
	c_dog_sets[torso_rot_x] = speed*10 * c_dog_sets[head_rot_x];
	c_dog_sets[tongue_rot_z] = speed*20 * c_dog_sets[head_rot_x];
	c_dog_sets[tail_rot_x] = speed*c_dog_sets[head_rot_x];


	if (frame < 100)
		frame += 1;
	else{
		ctmI = dog_trM;
		frame = 0;
		if (autoR)
			destination = vec3(1.0);
		else
			dog_rotM = get_speed_angle(position, vec3(me->position.x, me->position.y, me->position.z), interpspeed, cosV, sinV);

	}
	dog_trM = ctmI*dog_rotM*Translate(-interpspeed*frame*sinV, 0.0, -interpspeed*frame*cosV);
	position = mvmult(dog_trM, position);
	return dog_trM;
}

runningDog::runningDog(){
	dog_cube = new VBOCube();
	for (int i = 0; i<100; i++) {
		dog_sets[0][i] = 0.0, dog_sets[1][i] = 0.0, dog_sets[2][i] = 30.0, dog_sets[3][i] = -30.0, dog_sets[4][i] = 10.0, dog_sets[5][i] = -20.0, dog_sets[6][i] = 10.0;
	}
	c_dog_sets[0] = 0.0,
		c_dog_sets[1] = 0.0,
		c_dog_sets[2] = 0.0,
		c_dog_sets[3] = 0.0,
		c_dog_sets[4] = 0.0,
		c_dog_sets[5] = 0.0,
		c_dog_sets[6] = 0.0;
	c_dog_sets[7] = 100.0;
	c_dog_sets[8] = 150.0;
	c_dog_sets[9] = 200.0;
	c_dog_sets[10] = 300.0;
	c_dog_sets[11] = 0.0;
	c_dog_sets[12] = 0.0;
	c_dog_sets[13] = 0.0;

	position = vec4(0.0f, 0.0f, -30.0f, 0.0f);
	//dog_rotM = get_speed_angle(position, rand_destination(10), interpspeed);
};

void runningDog::render(){

	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 1);


	//model = mat4(1.0f);

	/******** base *******/
	// hierarchical transformation //*Translate(c_dog_sets[torso_trans_x] / 100, default_y, c_dog_sets[torso_trans_z] / 100)*RotateX(c_dog_sets[torso_rot_x])*RotateY(c_dog_sets[torso_rot_y])
	model = model*autorun(1.5)*RotateX(c_dog_sets[torso_rot_x]);;

	// instance
	models.push_back(model);
	model = model*Translate(0.0, 0.0, 0.0)*Scale(1.2, 1, 1.6);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 1.0, 1.0, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	mat4 ctm = model;

	/******** neck *******/
	// hierarchical transformation 
	model = model*Translate(0.0, 1.0, 0.65)*RotateX(c_dog_sets[head_rot_x])*RotateY(c_dog_sets[head_rot_y])*RotateZ(c_dog_sets[head_rot_z]);

	// instance
	models.push_back(model);
	model = model*Translate(0.0, 0.0, 0.0)*Scale(0.3, 1.0, 0.3);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.9, 0.9, 0.9, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();


	/******** head *******/
	// hierarchical transformation 
	model = model*Translate(0.0, 0.7, 0.0)*RotateX(c_dog_sets[head_rot_x])*RotateY(c_dog_sets[head_rot_y])*RotateZ(c_dog_sets[head_rot_z]);

	// instance
	models.push_back(model);
	model = model*Translate(0.0, 0.0, 0.0)*Scale(0.4, 0.5, 0.4);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 1.0, 1.0, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/******** left eye *******/
	models.push_back(model);
	model = model*Translate(-0.1, 0.2, 0.2)*Scale(0.1, 0.1, 0.1);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0, 0, 0, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/******** right eye *******/
	models.push_back(model);
	model = model*Translate(0.1, 0.2, 0.2)*Scale(0.1, 0.1, 0.1);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0, 0, 0, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();


	/******** left ear *******/
	models.push_back(model);
	model = model*Translate(-0.2, 0.2, 0.0)*Scale(0.1, 0.4, 0.25)*RotateZ(-30);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1, 1, 1, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/******** right ear *******/
	models.push_back(model);
	model = model*Translate(0.2, 0.2, 0.0)*Scale(0.1, 0.4, 0.25)*RotateZ(30);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1, 1, 1, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/******** mouse *******/
	// hierarchical transformation 
	model = model*Translate(0.0, -0.1, 0.3)*RotateX(c_dog_sets[head_rot_x])*RotateY(c_dog_sets[head_rot_y])*RotateZ(c_dog_sets[head_rot_z]);

	// instance
	models.push_back(model);
	model = model*Translate(0.0, 0.0, 0.0)*Scale(0.3, 0.3, 0.3);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.7, 0.7, 0.7, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();


	mat4 ctm2 =model;

	//tongue
	model = model*Translate(0.0, 0, 0.15)*RotateZ(c_dog_sets[tongue_rot_z]);

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.15, 0.3, 0.1);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.8, 0.2, 0.0, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	model = ctm2;
	/******** nose *******/
	// hierarchical transformation 
	model = model*Translate(0.0, 0.15, 0.15);

	// instance
	models.push_back(model);
	model = model*Translate(0.0, 0.0, 0.0)*Scale(0.1, 0.1, 0.1);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0, 0, 0, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	model = ctm;

	//leg
	/********Front left_upper *******/
	// hierarchical transformation 
	model = model*Translate(-0.5, -0.4, 0.7)*RotateX(50 * cos(c_dog_sets[front_left_leg] / 30));

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.9, 0.9, 0.9, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/********Front left_lower *******/
	// hierarchical transformation
	model = model*Translate(0.0, -0.6, 0.0)*RotateX(50 - 50 * cos(c_dog_sets[front_left_leg] / 30));

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.7, 0.7, 0.7, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	model = ctm;

	//leg
	/********Front right_upper *******/
	// hierarchical transformation 
	model = model*Translate(0.5, -0.4, 0.7)*RotateX(50 * cos(c_dog_sets[front_right_leg] / 30));

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.9, 0.9, 0.9, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/********Front right_lower *******/
	// hierarchical transformation
	model = model*Translate(0.0, -0.6, 0.0)*RotateX(50 - 50 * cos(c_dog_sets[front_right_leg] / 30));

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.7, 0.7, 0.7, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	model = ctm;

	//leg
	/********back left_upper *******/
	// hierarchical transformation 
	model = model*Translate(-0.5, -0.4, -0.7)*RotateX(50 * cos(c_dog_sets[back_left_leg] / 30));

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.9, 0.9, 0.9, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/********back left_lower *******/
	// hierarchical transformation
	model = model*Translate(0.0, -0.6, 0.0)*RotateX(50 - 50 * cos(c_dog_sets[back_left_leg] / 30));

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.7, 0.7, 0.7, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	model = ctm;

	//leg
	/********back right_upper *******/
	// hierarchical transformation 
	model = model*Translate(0.5, -0.4, -0.7)*RotateX(50 * cos(c_dog_sets[back_right_leg] / 30));
	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.9, 0.9, 0.9, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/********back right_lower *******/
	// hierarchical transformation
	model = model*Translate(0.0, -0.6, 0.0)*RotateX(50 - 50 * cos(c_dog_sets[back_right_leg] / 30));

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.7, 0.7, 0.7, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	model = ctm;

	/********tail_upper *******/
	// hierarchical transformation 
	model = model*Translate(0, 0.4, -0.7)*RotateZ(50 * c_dog_sets[head_rot_z])*RotateX(c_dog_sets[tail_rot_x] + 120.0);

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.9, 0.9, 0.9, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	/********tail_lower *******/
	// hierarchical transformation
	model = model*Translate(0.0, -0.6, 0.0)*RotateX(50 * c_dog_sets[head_rot_z])* RotateX(c_dog_sets[tail_rot_x] + 30);;

	// instance 
	models.push_back(model);
	model = model*Translate(0, -0.3, 0)*Scale(0.2, 0.6, 0.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.7, 0.7, 0.7, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	dog_cube->render();
	model = models.back(); models.pop_back();

	model = ctm;

	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 0);

}