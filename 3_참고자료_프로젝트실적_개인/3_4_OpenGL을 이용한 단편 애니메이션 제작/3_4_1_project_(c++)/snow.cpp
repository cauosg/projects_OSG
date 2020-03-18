#include "diging.h"
#include "vbomesh.h"
#include "vbosphere.h"
#include "snow.h"
#include "base.h"
#define MAX_PARTICLE 1000

extern _season season[4];
extern int nowseason;
extern vec3 eye;
extern GLint mv_location;
extern mat4 view;
extern mat4 model;
extern GLuint rendering_program;
vec3 particlePos[MAX_PARTICLE];
float ii = 0.0f;
float diffy = 0;
float diffz = 0;

snow::snow(){
	init();
}

void snow::init(){
	for (int i = 0; i < MAX_PARTICLE; i++){
		diffy = rand() % 900;
		diffz = -rand() % 2000;
		particlePos[i] = vec3(i * 2 - MAX_PARTICLE / 2, diffy, diffz + 1000);
	}

}

void snow::render(){

	// Disable depth test
	glDisable(GL_DEPTH_TEST);
	// Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable Point Size. Enable Point Coord.
	glEnable(GL_PROGRAM_POINT_SIZE); //(20.0f / abs(eye.z - particlePos[i].z));
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);


	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 2);
	glUniform3fv(glGetUniformLocation(rendering_program, "leafcolor"), 1, season[nowseason].tex_color);
	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), season[nowseason].tex_num);


	for (int i = 10; i < MAX_PARTICLE; i++){

		particlePos[i].y -= ii;
		if (particlePos[i].y < -60)
			particlePos[i].y = 450;

		model = Translate(particlePos[i]);
		glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
		
		glPointSize(20000.0f / length(eye - particlePos[i]));

		glBegin(GL_POINTS);
		glVertex3fv(particlePos[i]);
		glEnd();
	}

	//ii = 2.0f;
	ii = season[nowseason].fallingV;
	if (ii > 2000000)
	{
		ii = 0.1;
	}
	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 0);

}