#include "diging.h"
#include "vbomesh.h"
#include "vbosphere.h"
#include "flower.h"

extern GLint mv_location;
extern mat4 view;
extern mat4 model;
extern GLuint rendering_program;
VBOMesh *leaf;
VBOMesh *stem;
VBOMesh *sp;

flower::flower(){
	leaf = new VBOMesh("flower/leaf.obj", false, false, false);
	stem = new VBOMesh("flower/stem.obj", false, false, false);
	sp = new VBOMesh("flower/sp.obj", false, false, false);
}

void flower::render(){
	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 1);


	//model = model*Translate(0.0, -0.2, 0.0)*Scale(1.8, 2.2, 1.2);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.1, 0.5, 0.1, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	stem->render();

	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 1.0, 1.0, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	leaf->render();

	//model = Translate(-5.5, 15, -5.5)*Scale(2, 0.5, 2);;
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(1.0, 1.0, 0.0, 1.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	sp->render();

	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 0);
}