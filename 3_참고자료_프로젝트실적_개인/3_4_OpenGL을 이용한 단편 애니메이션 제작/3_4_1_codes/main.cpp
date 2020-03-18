#include "diging.h"
#include "bmpreader.h"
#include "vbomesh.h"
#include "vbosphere.h"
#include "runningDog.h"
#include "particle.h"
#define NULL_TEXTURE 9
vec3 pos1, pos2;
mat4 view;
mat4 projection;
mat4 model;
GLfloat Lightintensity = 0.9f;
GLfloat LightX = 50.0f;
GLfloat LightY = 0.0f;
//vec3 eye = { 19.0f, 50.0f, 65.5f };
//vec3 at = { 19.0f, 12.25f, 0.0f };
//vec3 up = { 0.0f, 1.0f, 0.0f };
vec3 eye = { 0.0f, 0.0f, 10.0f };
vec3 at = { 0.0f, 0.0f, 0.0f };
vec3 up = { 0.0f, 1.0f, 0.0f };

void startup();
void render();
void shutdown();
extern void version();
extern GLuint compile_shaders(GLchar* vsource, GLchar* fsource);

GLuint rendering_program;
Particle *P1[3];
runningDog *Dog;
VBOMesh *spaceship;

float angle = 0.0f;

GLint mv_location;
GLint proj_location;

GLuint tex_object[31];

const int WIDTH = 1200; const int HEIGHT = 900;

void startup()
{
	/******* OpenGL Initialization */
	glEnable(GL_DEPTH_TEST); //glDepthFunc(GL_LEQUAL);

	/**** Shaders as variables */
	//rendering_program = compile_shaders("particles_vs.glsl", "particles_fs.glsl");
	rendering_program = compile_shaders("v_shader.glsl", "f_shader.glsl");
	glUseProgram(rendering_program);

	// modeling
	Dog = new runningDog();
	P1[0] = new Particle();
	P1[1] = new Particle();
	P1[2] = new Particle();

	// Load texture file
	GLuint w, h;
	glActiveTexture(GL_TEXTURE0);
	tex_object[0] = BMPReader::loadTex("texture/sun.bmp", w, h);
	glActiveTexture(GL_TEXTURE10);
	tex_object[10] = BMPReader::loadTex("texture/sun_normal.bmp", w, h);
	glActiveTexture(GL_TEXTURE9);
	tex_object[9] = NULL;//BMPReader::loadTex("texture/earth_norm.bmp", w, h);
	glActiveTexture(GL_TEXTURE20);
	//tex_object[20] = BMPReader::loadTex("fire.bmp");
	tex_object[20] = BMPReader::loadTex("Particle/fire2.bmp");


	
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

	// lighting
	glUniform3fv(glGetUniformLocation(rendering_program, "Light.Intensity"), 1, vec3(Lightintensity, Lightintensity, Lightintensity));
	glUniform4f(glGetUniformLocation(rendering_program, "Light.Position"), 1, LightX, LightY, 0);//view*vec4(cos(-angle)*100.0, 60.0, sin(-angle)*100.0, 1.0));
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ks"), 0.3f, 0.3f, 0.3f);
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ka"), 0.2f, 0.2f, 0.2f);
	glUniform1f(glGetUniformLocation(rendering_program, "Material.Shininess"), 100.0f);

	model = mat4(1.0);
	model = Translate(0.0f, -0.0, 0.0) * Rotate(angle, vec3(0.0f, 1.0f, 0.0f));

	view = LookAt(eye, at, up);
	projection = Perspective(90.0f, 1.33f, 10.0f, 1000.0f);
	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

	glUniformMatrix4fv(mv_location, 1, GL_TRUE, model*view);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), NULL_TEXTURE);
	glUniform1i(glGetUniformLocation(rendering_program, "NormalMapTex"), NULL_TEXTURE);
	model = Translate(vec3(0.0f, 0.0f, 0.0f))*Scale(5.0f);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);

	Dog->render();
	model = Scale(0.01,0.1,0.1);


	model = Scale(0.1);
	P1[0]->render();
	model = Scale(0.12);
	P1[1]->render();
	model = Scale(0.07);
	P1[2]->render();
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
	case 'x': P1[0]->initBuffers(); P1[0]->zerot(); break;
	case 'c': P1[1]->initBuffers(); P1[1]->zerot(); break;
	case 'v': P1[2]->initBuffers(); P1[2]->zerot(); break;
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
