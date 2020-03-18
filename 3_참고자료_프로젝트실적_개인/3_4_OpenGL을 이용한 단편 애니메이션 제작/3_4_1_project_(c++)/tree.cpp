#include "tree.h"
#include "base.h"

extern int nowseason;
extern _season season[10];
extern GLint mv_location;
extern mat4 view;
extern mat4 model;
extern GLuint rendering_program;
extern vec3 eye;
VBOMesh *tr;
float time = 0.0f;
float dx = 0;
float dy = 0;
float dz = 0;
float psize[MAX_LEAFS];
float initv[MAX_LEAFS];

vec3 leafPos[MAX_LEAFS];
inline float randFloat(int k) { return ((float)rand() / (RAND_MAX/k)); }

tree::tree(){
	tr = new VBOMesh("tree/tree.obj", false, false, false);
	init();
}

void tree::init(){
	float treerad = 200;
	for (int i = 0; i < MAX_LEAFS; i++){
		float theta = randFloat(3*rand());
		float omega = randFloat(rand()/2);

		dx = treerad * sinf(theta)*cos(omega);
		dy = treerad * sinf(theta)*sin(omega);
		dz = treerad * cosf(theta);
		psize[i] = rand() % 5 + 5;
		initv[i] = rand() % 300;
		initv[i] /= 100.0;
		leafPos[i] = vec3(dx / 10, dy / 10.0, dz / 10.0);
	}
}

void tree::render(){
	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 1);

	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.4, 0.2, 0.1, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	tr->render();


	// Disable depth test
	glDisable(GL_DEPTH_TEST);
	// Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable Point Size. Enable Point Coord.
	glEnable(GL_PROGRAM_POINT_SIZE); //(20.0f / abs(eye.z - leafPos[i].z));
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);


	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 2);
	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), 20);
	model *= Translate(0, 80, -10);
	mat4 ctmtree = model;
	glUniform1i(glGetUniformLocation(rendering_program, "option"), 1);
	glUniform1f(glGetUniformLocation(rendering_program, "Time"), time);
	glUniform1f(glGetUniformLocation(rendering_program, "poslimit"), -90);
	glUniform3fv(glGetUniformLocation(rendering_program, "leafcolor"),1,season[nowseason].leafcolor);
	for (int i = 10; i < MAX_LEAFS; i++){
		int rempos = (int)(leafPos[i].y - time) % 100;
		if (rempos < -100)
			leafPos[i].y += 100;

		model = ctmtree*Translate(leafPos[i].x , leafPos[i].y, leafPos[i].z);
		if (falling)
			glUniform1f(glGetUniformLocation(rendering_program, "initvel"), initv[i]);
		else
			glUniform1f(glGetUniformLocation(rendering_program, "initvel"), 0);

		glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
		glPointSize(psize[i] * 200.0f / length(eye - leafPos[i]));

		glBegin(GL_POINTS);
		glVertex3fv(leafPos[i]);
		glEnd();
	}
	glUniform1i(glGetUniformLocation(rendering_program, "option"), 0);
	if (falling)
		time += 0.20f;
	else
		time = 0;
	if (time > 2000000)
	{
		time = 0.1;
	}
	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 0);
}