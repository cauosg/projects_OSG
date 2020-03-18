#include "diging.h"
#include "bmpreader.h"
#include "vbomesh.h"
#include "vbosphere.h"
#include "runningDog.h"
#include "particle.h"
vec3 pos1, pos2;
mat4 view;
mat4 model;
GLfloat Lightintensity = 0.9f;
GLfloat LightX = 50.0f;
GLfloat LightY = 0.0f;
//vec3 eye = { 19.0f, 50.0f, 65.5f };
//vec3 at = { 19.0f, 12.25f, 0.0f };
//vec3 up = { 0.0f, 1.0f, 0.0f };
vec3 eye = { 0.0f, 0.0f, 100.0f };
vec3 at = { 0.0f, 0.0f, 0.0f };
vec3 up = { 0.0f, 1.0f, 0.0f };

void startup();
void render();
void shutdown();
extern void version();
extern GLuint compile_shaders(GLchar* vsource, GLchar* fsource);

GLuint rendering_program;
Particle *P1;
runningDog *Dog;
VBOMesh *spaceship;
VBOSphere *solarSystem[9];
GLfloat Size[9] = {
	29.25f,
	0.38f,
	0.95f,
	1.00f,
	0.53f,
	10.97f,
	9.14f,
	3.98f,
	3.87f
};
vec3 PlanetPos[9] = {
	vec3{ 0.0, 0.0, 0.0 },
	vec3{ 27.0, 0.0, 0.0 },
	vec3{ 32.0, 0.0, 0.0 },
	vec3{ 35.0, 0.0, 0.0 },
	vec3{ 40.0, 0.0, 0.0 },
	vec3{ 60.0, 0.0, 0.0 },
	vec3{ 80.0, 0.0, 0.0 },
	vec3{ 95.0, 0.0, 0.0 },
	vec3{ 100.0, 0.0, 0.0 }
};

GLfloat angleSpeed[9] = {
	0.2f,
	0.7f,
	1.2f,
	1.4f,
	1.1f,
	2.0f,
	0.7f,
	1.7f,
	0.5f
};
float angle = 0.0f;

GLint mv_location;
GLint proj_location;

GLuint tex_object[14];

const int WIDTH = 1200; const int HEIGHT = 900;

void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); //glDepthFunc(GL_LEQUAL);

	/**** Shaders as variables */
	//rendering_program = compile_shaders("particles_vs.glsl", "particles_fs.glsl");
	rendering_program = compile_shaders("normalv.glsl", "normalf.glsl");
	glUseProgram(rendering_program);

	// modeling
	Dog = new runningDog();
	spaceship = new VBOMesh("spaceship.obj", false, true, true);
	for (int i = 0; i < 9; i++){
		solarSystem[i] = new VBOSphere(Size[i], 1000, 1000);
	}
	P1 = new Particle();

	// Load texture file
	GLuint w, h;
	glActiveTexture(GL_TEXTURE0);
	tex_object[0] = BMPReader::loadTex("texture/sun.bmp", w, h);
	glActiveTexture(GL_TEXTURE1);
	tex_object[1] = BMPReader::loadTex("texture/mercury.bmp", w, h);
	glActiveTexture(GL_TEXTURE2);
	tex_object[2] = BMPReader::loadTex("texture/venus.bmp", w, h);
	glActiveTexture(GL_TEXTURE3);
	tex_object[3] = BMPReader::loadTex("texture/earth_text.bmp", w, h);
	glActiveTexture(GL_TEXTURE4);
	tex_object[4] = BMPReader::loadTex("texture/neptune.bmp", w, h);
	glActiveTexture(GL_TEXTURE5);
	tex_object[5] = BMPReader::loadTex("texture/mars.bmp", w, h);
	glActiveTexture(GL_TEXTURE6);
	tex_object[6] = BMPReader::loadTex("texture/jupiter.bmp", w, h);
	glActiveTexture(GL_TEXTURE7);
	tex_object[7] = BMPReader::loadTex("texture/saturn.bmp", w, h);
	glActiveTexture(GL_TEXTURE8);
	tex_object[8] = BMPReader::loadTex("texture/uranus.bmp", w, h);
	glActiveTexture(GL_TEXTURE9);
	tex_object[9] = NULL;//BMPReader::loadTex("texture/earth_norm.bmp", w, h);
	glActiveTexture(GL_TEXTURE10);
	tex_object[10] = BMPReader::loadTex("texture/sun_normal.bmp", w, h);
	glActiveTexture(GL_TEXTURE11);
	tex_object[11] = BMPReader::loadTex("texture/spaceship.bmp", w, h);

}

void idle(){
	angle += 0.20;
	if (angle > 3600.0) angle -= 3600.0;
	Lightintensity = 0.4 + 0.6*abs(cos(angle / 10));
	LightX = 100 * cos(angle / 10);
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glUniform1i(glGetUniformLocation(rendering_program, "Material.coloring"), 0);

	// lighting
	glUniform3fv(glGetUniformLocation(rendering_program, "Light.Intensity"), 1, vec3(Lightintensity, Lightintensity, Lightintensity));
	glUniform4f(glGetUniformLocation(rendering_program, "Light.Position"), 1, PlanetPos[0].x + LightX, PlanetPos[0].y + LightY, PlanetPos[0].z);//view*vec4(cos(-angle)*100.0, 60.0, sin(-angle)*100.0, 1.0));
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ks"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ka"), 0.2f, 0.2f, 0.2f);
	glUniform1f(glGetUniformLocation(rendering_program, "Material.Shininess"), 100.0f);

	model = mat4(1.0);
	model = Translate(0.0f, -0.0, 0.0) * Rotate(angle, vec3(0.0f, 1.0f, 0.0f));

	view = LookAt(eye, at, up);
	mat4 projection = Perspective(90.0f, 1.33f, 10.0f, 1000.0f);
	//mat4 projection = Ortho(-10.0, 10.0, -10.0, 10.0, -10, 100);
	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

	glUniformMatrix4fv(mv_location, 1, GL_TRUE, model*view);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), 0);
	glUniform1i(glGetUniformLocation(rendering_program, "NormalMapTex"), 10);
	model = Translate(PlanetPos[0]) * RotateY(angle) * RotateX(90.0f);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	solarSystem[0]->render();

	glUniform1i(glGetUniformLocation(rendering_program, "NormalMapTex"), 9);
	for (int i = 1; i < 9; i++){
		glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), i);
		model = RotateY((angle + 90.0)*angleSpeed[i]) * Translate(PlanetPos[i]) * RotateX(90.0f) * RotateZ(angle);
		glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);

		solarSystem[i]->render();
	}


	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), 11);
	model = Translate(14.0f, 55.0f, 60.0f * cos(angle / 50)) * RotateZ(angle) * RotateY(cos(angle / 50)) *  Scale(0.003);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);

	spaceship->render();

	model = Translate(vec3(50.0f, 0.0f, 0.0f))*Scale(10.0f);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);

	Dog->render();



	model = Scale(1.0f);
	glUniform1i(glGetUniformLocation(rendering_program, "Material.coloring"), 2);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	P1->render();


	glutSwapBuffers();
}

void shutdown()
{
	glDeleteTextures(2, tex_object);
	glDeleteProgram(rendering_program);
}


void mouse(int button, int state, int x, int y)
{

	float u, v;
	u = (float)x / (WIDTH / 2) - 1.0;
	v = (float)(HEIGHT - y) / (HEIGHT / 2) - 1.0;

	if (state == GLUT_DOWN) {
		pos1.x = u;
		pos1.y = v;
		pos1.z = sqrtf(1.0 - u*u - v*v);
	}

	if (state == GLUT_UP) {
		pos2.x = u;
		pos2.y = v;
		pos2.z = sqrtf(1.0 - u*u - v*v);

		if (pos1.x == pos2.x || pos1.y == pos2.y)
			return;
		float scailfactor = (1 - (pos2.x - pos1.x + pos2.y + -pos1.y) / 2);
		vec3 n = cross(pos1, pos2);
		float theta = 180.0*length(n) / M_PI;
		mat4 now(1.0);
		vec4 temp;
		switch (button) {
		case GLUT_LEFT_BUTTON:
			temp = mvmult(Rotate(theta / 10, n), vec4(eye.x, eye.y, eye.z, 1));
			eye.x = temp.x;
			eye.y = temp.y;
			eye.z = temp.z;
			temp = mvmult(Rotate(theta / 10, n), vec4(at.x, at.y, at.z, 1));
			at.x = temp.x;
			at.y = temp.y;
			at.z = temp.z;
			break;
		case GLUT_RIGHT_BUTTON:
			temp = mvmult(Scale(vec3(scailfactor, scailfactor, scailfactor)), vec4(eye.x, eye.y, eye.z, 1));
			eye.x = temp.x;
			eye.y = temp.y;
			eye.z = temp.z;
			break;
		case GLUT_MIDDLE_BUTTON:
			temp = mvmult(Translate(vec3(pos1.x - pos2.x, pos1.y - pos2.y, pos1.z - pos2.z)), vec4(eye.x, eye.y, eye.z, 1));
			eye.x = temp.x;
			eye.y = temp.y;
			eye.z = temp.z;
			temp = mvmult(Translate(vec3(pos1.x - pos2.x, pos1.y - pos2.y, pos1.z - pos2.z)), vec4(at.x, at.y, at.z, 1));
			at.x = temp.x;
			at.y = temp.y;
			at.z = temp.z;
			break;
		}
	}	printf("%f on\n", eye.y);
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'q': Lightintensity += 0.1; break;
	case 'e': Lightintensity -= 0.1; break;
	case 'w': LightX += 10.0f; break;
	case 's': LightX -= 10.0; break;
	case 'a': LightY -= 10.0; break;
	case 'd': LightY += 10.0; break;
	case 'z': Dog->autorun(2.5f); break;
	default:
		break;
	}
	glutPostRedisplay();
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("Normal Mapping");
	glewInit();

	version();
	startup();
	glutDisplayFunc(render);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();
	shutdown();
	return 0;
}
