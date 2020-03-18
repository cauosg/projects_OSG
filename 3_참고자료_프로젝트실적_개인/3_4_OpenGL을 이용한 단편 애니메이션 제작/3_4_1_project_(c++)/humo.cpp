#include "humo.h"
#include "vbocube.h"
#include "vbosphere.h"
#include "VBOMesh.h"
#include "interp.h"
VBOMesh* hat;
extern GLint mv_location;
extern mat4 view;
extern mat4 model;
extern GLuint rendering_program;
extern bool autoR;
// animation  변수

bool animate;
int timeframe = 0;
const int NumOptions = 26; // option: head x trans, z trans, y 축회전, torso z 회전, x 회전, right arm z 회전, x 회전
float option[26][100];
float c_option[26];

vector <mat4> h_models;
VBOCube *cube;
VBOSphere *sphere;
mat4 rotM(1.0);
mat4 trM(1.0);

void humo::reset(){


}

humo::humo(){
	hat = new VBOMesh("hat/hat.obj", true, false, false);
	cube = new VBOCube();
	sphere = new VBOSphere(1.0f, 100, 100);
	for (int i = 0; i<100; i++) {
		option[0][i] = 0.0, option[1][i] = 0.0, option[2][i] = 30.0, option[3][i] = -30.0, option[4][i] = 10.0, option[5][i] = -20.0, option[6][i] = 10.0;
	}
	c_option[0] = 0.0,
		c_option[1] = 0.0,
		c_option[2] = 0.0,
		c_option[3] = 30.0,
		c_option[4] = 0.0,
		c_option[5] = 10.0,
		c_option[6] = 0.0;
	c_option[7] = -30.0;
	c_option[8] = 0.0;
	c_option[9] = -10.0;
	c_option[10] = 0.0;
	c_option[11] = 0.0;
	c_option[12] = 10.0;
	c_option[13] = 0.0;
	c_option[14] = 0.0;
	c_option[15] = 0.0;
	c_option[16] = -10.0;
	c_option[17] = 0.0;
	c_option[18] = 0.0;
	c_option[19] = 0.0;
	c_option[20] = 0.0;
	c_option[21] = 0.0;
	c_option[22] = 0.0;
	c_option[23] = 0.0;
	c_option[24] = 0.0;
	c_option[25] = 200.0;

	position = vec4(0.0f,0.0f,10.0f,0.0f);
	//rotM = get_speed_angle(position, rand_destination(5), interpspeed);
}
mat4 humo::autorun(float speed){
	speed = interpspeed*speed;
	static float cosV, sinV;
	static float distz = 0.0;
	static float frame = 100;
	static float time = 0.0f;
	static mat4 ctmI(1.0);

	float angle = cos(time);

	c_option[left_upper_arm_rot_x] = speed * 30 * angle;
	c_option[right_upper_arm_rot_x] = -speed * 30 * angle;
	c_option[left_lower_arm_rot_x] = speed * 15 * angle-15;
	c_option[right_lower_arm_rot_x] = -speed * 15 * angle-15;

	c_option[left_upper_leg_rot_x] = speed * 30 * angle;
	c_option[right_upper_leg_rot_x] = -speed * 30 * angle;
	c_option[left_lower_leg_rot_x] = 15 + speed * 15 * angle;
	c_option[right_lower_leg_rot_x] = 15 - speed * 15 * angle;
	c_option[left_foot_rot_x] = speed * 15 * angle - 15;
	c_option[right_foot_rot_x] = speed * 15 * angle - 15;

	c_option[torso_pos_y] = speed*abs(angle*0.3);
		
	if (frame < 100){
		time += 0.7;
		frame += 1;
	}
		else{
			ctmI = trM;
			frame = 0;
			rotM = get_speed_angle(position, rand_destination(5), interpspeed, cosV, sinV);
			
		}
		trM = ctmI*Translate(interpspeed*frame*sinV, 0.0, interpspeed*frame*cosV)*rotM;
		position = mvmult(trM, position);
		return trM;
}

void humo::render()
{
	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 1);

	/******** torso *******/
	// hierarchical transformation 
	//if (autoR)
	model = model*Translate(0.0, c_option[torso_pos_y], 0.0)*autorun(9.0);//

	// instance
	h_models.push_back(model);
	model = model*Translate(0.0, -0.2, 0.0)*Scale(1.8, 2.2, 1.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.0, 0.2, 0.5, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	cube->render();
	model = h_models.back(); h_models.pop_back();

	mat4 ctm = model;

	//neck
	// hierarchical transformation 
	model = model*Translate(0.0, 1.0, 0.0)*RotateX(c_option[neck_rot_x]);
	// instance
	h_models.push_back(model);
	model = model*Translate(0.0, 0.0, 0.0)*Scale(0.5, 1.0, 0.5);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 0.8, 0.8, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	cube->render();
	model = h_models.back(); h_models.pop_back();

	/******** head *******/
	// hierarchical transformation 
	model = model*Translate(0.0, 0.5, 0.0)*RotateX(c_option[head_rot_x])*RotateY(c_option[head_rot_y]);

	// instance
	h_models.push_back(model);
	model = model*Translate(0.0, 0.8, 0.0)*Scale(1.0, 1.0, 1.0);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 0.8, 0.8, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	sphere->render();
	model = h_models.back(); h_models.pop_back();

	//hat
	h_models.push_back(model);
	model = model*Translate(0.0, 1.5, 0.0)*Scale(0.085);
	//glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 0);
	//glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), 1);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.4, 0.7, 0.8, 1.0));
	hat->render();
	model = h_models.back(); h_models.pop_back();
	//glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 1);

	//left eye
	h_models.push_back(model);
	model = model*Translate(-0.3, 0.9, 1.0)*Scale(0.05);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.0, 0.0, 0.0, 1.0));
	sphere->render();
	model = h_models.back(); h_models.pop_back();

	//right eye
	h_models.push_back(model);
	model = model*Translate(0.3, 0.9, 1.0)*Scale(0.05);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.0, 0.0, 0.0, 1.0));
	sphere->render();
	model = h_models.back(); h_models.pop_back();

	////mouse
	//h_models.push_back(model);
	//model = model*Translate(0.0, 0.4, 0.65)*Scale(0.5, 0.1, 0.4);
	//glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	//glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.0, 0.0, 0.0, 1.0));
	//cube->render();
	//model = h_models.back(); h_models.pop_back();



	model = ctm;


	/******** left_upper_arm *******/
	// hierarchical transformation 
	model = model*Translate(0.9 + 0.2, 0.9, 0.0)*RotateZ(c_option[left_upper_arm_rot_z])*RotateY(c_option[left_upper_arm_rot_y])*RotateX(c_option[left_upper_arm_rot_x]);

	// instance 
	h_models.push_back(model);
	model = model*Translate(0, -0.6, 0)*Scale(0.4, 1.2, 0.4);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.0, 0.2, 0.5, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	/******** left_lower_arm *******/
	// hierarchical transformation
	model = model*Translate(0.0, -1.2, 0.0)*RotateX(c_option[left_lower_arm_rot_x]);//*Translate(0.0, -0.8, 0.0);
	// instance 
	h_models.push_back(model);
	model = model*Translate(0, -0.2, 0)*Scale(0.4, 0.8, 0.4);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.0, 0.2, 0.5, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();
	// instance 
	h_models.push_back(model);
	model = model*Translate(0, -0.8, 0)*Scale(0.3, 0.8, 0.2);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 0.8, 0.8, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	model = ctm;

	/******** right_upper_arm *******/
	// hierarchical transformation 
	model = model*Translate(-0.9 - 0.2, 0.9, 0.0)*RotateZ(c_option[right_upper_arm_rot_z])*RotateY(c_option[right_upper_arm_rot_y])*RotateX(c_option[right_upper_arm_rot_x]);

	// instance 
	h_models.push_back(model);
	model = model*Translate(0, -0.6, 0)*Scale(0.4, 1.2, 0.4);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.0, 0.2, 0.5, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	/******** right_lower_arm *******/
	// hierarchical transformation
	model = model*Translate(0.0, -1.2, 0.0)*RotateX(c_option[right_lower_arm_rot_x]);
	// instance 
	h_models.push_back(model);
	model = model*Translate(0, -0.2, 0)*Scale(0.4, 0.8, 0.4);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.0, 0.2, 0.5, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();
	// instance 
	h_models.push_back(model);
	model = model*Translate(0, -0.8, 0)*Scale(0.3, 0.8, 0.2);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 0.8, 0.8, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	model = ctm;

	/******** left_upper_leg *******/
	// hierarchical transformation 
	model = model*Translate(0.5, -1.0, 0.0)*RotateX(c_option[left_upper_leg_rot_x])*RotateY(c_option[left_upper_leg_rot_y]);

	// instance 
	h_models.push_back(model);
	model = model*Translate(0.0, -0.5, 0.0)*Scale(0.6, 1.0, 0.4);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.3, 0.3, 0.3, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	/******** left_lower_leg *******/
	// hierarchical transformation
	model = model*Translate(0.0, -1.0, 0.0)*RotateX(c_option[left_lower_leg_rot_x]);

	// instance 
	h_models.push_back(model);
	model = model*Translate(0.0, -0.5, 0.0)*Scale(0.3, 1.0, 0.3);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 0.8, 0.8, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	/******** left_foot *******/
	// hierarchical transformation
	model = model*Translate(0.0, -1.0, 0.0)*RotateX(c_option[left_foot_rot_x]);

	// instance 
	h_models.push_back(model);
	model = model*Translate(0.0, -0.1, 0.2)*Scale(0.4, 0.2, 0.7);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.5, 0.2, 0.0, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	model = ctm;

	/******** right_upper_leg *******/
	// hierarchical transformation 
	model = model*Translate(-0.5, -1.0, 0.0)*RotateX(c_option[right_upper_leg_rot_x])*RotateY(c_option[right_upper_leg_rot_y]);

	// instance 
	h_models.push_back(model);
	model = model*Translate(0.0, -0.5, 0.0)*Scale(0.6, 1.0, 0.4);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.3, 0.3, 0.3, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	/******** right_lower_leg *******/
	// hierarchical transformation
	model = model*Translate(0.0, -1.0, 0.0)*RotateX(c_option[right_lower_leg_rot_x]);

	// instance 
	h_models.push_back(model);
	model = model*Translate(0.0, -0.5, 0.0)*Scale(0.3, 1.0, 0.3);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 0.8, 0.8, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	/******** right_foot *******/
	// hierarchical transformation
	model = model*Translate(0.0, -1.0, 0.0)*RotateX(c_option[right_foot_rot_x]);

	// instance 
	h_models.push_back(model);
	model = model*Translate(0.0, -0.1, 0.2)*Scale(0.4, 0.2, 0.7);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.5, 0.2, 0.0, 1.0));
	cube->render();
	model = h_models.back(); h_models.pop_back();

	model = ctm;

	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 0);
}