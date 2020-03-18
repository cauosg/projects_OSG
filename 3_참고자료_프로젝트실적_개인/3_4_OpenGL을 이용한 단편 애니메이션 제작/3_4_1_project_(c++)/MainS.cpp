#include "base.h"
#define NULL_TEXTURE 9
int spotcont = 0;
int phongcont = 0;
vec3 pos1, pos2;
mat4 view;
mat4 projection;
mat4 model;
GLfloat Lightintensity = 1.0f;
GLfloat LightX = 0.0f;
GLfloat LightY = 0.0f;

vec3 eye = { 50.0f, 10.0f, 20.0f };
vec3 at = { 0.0f, 0.0f, 0.0f };
vec3 up = { 0.0f, 1.0f, 0.0f };

_season season[4];
_climate climate[10];
int nowclimate = 3;
int nowseason = 0;


void startup();
void render();
void shutdown();
extern void version();
extern GLuint compile_shaders(GLchar* vsource, GLchar* fsource);
bool autoR;
GLuint rendering_program;
Particle *P1[3];
runningDog *Dog;
VBOSphere *sp;
VBOPlane *plane;
humo *me;
flower *f1;
tree *tree1;
snow *snowy;
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
	rendering_program = compile_shaders("v_shader.glsl", "f_shader.glsl");
	glUseProgram(rendering_program);

	// modeling
	me = new humo();
	Dog = new runningDog();
	P1[0] = new Particle();
	P1[1] = new Particle();
	P1[2] = new Particle();
	sp = new VBOSphere(1.0,1000,1000);
	snowy = new snow();
	plane = new VBOPlane(100.0,100.0,1,1);


	f1 = new flower();
	tree1 = new tree();
	// Load texture file
	GLuint w, h;
	glActiveTexture(GL_TEXTURE0);
	tex_object[0] = BMPReader::loadTex("texture/sun.bmp", w, h);
	glActiveTexture(GL_TEXTURE1);
	tex_object[1] = BMPReader::loadTex("hat/pattern2.bmp", w, h);
	glActiveTexture(GL_TEXTURE10);
	tex_object[10] = BMPReader::loadTex("texture/sun_normal.bmp", w, h);
	glActiveTexture(GL_TEXTURE9);
	tex_object[9] = NULL;
	glActiveTexture(GL_TEXTURE20);
	tex_object[20] = BMPReader::loadTex("Particle/fire2.bmp");
	glActiveTexture(GL_TEXTURE21);
	tex_object[21] = BMPReader::loadTex("Particle/snow2.bmp");
	glActiveTexture(GL_TEXTURE22);
	tex_object[22] = BMPReader::loadTex("Particle/rain2.bmp");


	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	// Lighting
	glUniform3f(glGetUniformLocation(rendering_program, "Spot.Intensity"), 1.0, 1.0, 1.0);
	glUniform1f(glGetUniformLocation(rendering_program, "Spot.exponent"), 3.0f);
	glUniform1f(glGetUniformLocation(rendering_program, "Spot.cutoff"), 50.0f);
	climate[0].Ka = vec3(0.4, 0.4, 0.4);
	climate[0].Ks = vec3(0.8, 0.6, 0.6);
	climate[1].Ka = vec3(0.1, 0.1, 0.1);
	climate[1].Ks = vec3(0.8, 0.8, 0.8);


	season[0].leafcolor = vec3(4.0, 0.7, 5.0);
	season[0].tex_num = 22;
	season[0].tex_color = vec3(1.0, 1.0, 1.0);
	season[0].fallingV = 2.0;
	season[1].leafcolor = vec3(1.0, 1.0, 1.0);
	season[1].tex_num = 22;
	season[1].tex_color = vec3(1.0, 1.0, 1.0);
	season[1].fallingV = 7.0;
	season[2].leafcolor = vec3(3.0, 1.0, 0.0);
	season[2].tex_num = 22;
	season[2].tex_color = vec3(1.0, 1.0, 1.0);
	season[2].fallingV = 2.0;
	season[3].leafcolor = vec3(1.0, 0.2, 0.0);
	season[3].tex_num = 21;
	season[3].tex_color = vec3(1.0, 1.0, 1.0);
	season[3].fallingV = 2.0;
}


void idle(){
	if (autoR)
		eye = vec3(50 * cos(angle/2), 10+10*cos(angle/2), 50 * sin(angle/2));
	angle += 0.020;
	if (angle > 3600.0) 
		angle -= 3600.0;
	LightX = 100 * cos(angle / 10);
	LightY = 100 * sin(angle / 10);
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.7f, 1.0f, 1.0f);

	// lighting
	glUniform3fv(glGetUniformLocation(rendering_program, "Light.Intensity"), 1, vec3(Lightintensity, Lightintensity, Lightintensity));
	glUniform4f(glGetUniformLocation(rendering_program, "Light.Position"), 1, LightX, LightY, 50);
	glUniform3fv(glGetUniformLocation(rendering_program, "Material.Ks"), 1, climate[nowclimate].Ks);
	glUniform3fv(glGetUniformLocation(rendering_program, "Material.Ka"), 1, climate[nowclimate].Ka);
	glUniform1f(glGetUniformLocation(rendering_program, "Material.Shininess"), 0.5f);

	// Lighting
	vec4 lightPos = vec4(15 * cos(angle), 45.0f, 15 * sin(angle), 1.0f);
	vec4 direction = view*vec4(-lightPos);
	glUniform4fv(glGetUniformLocation(rendering_program, "Spot.Position"), 1, view*lightPos);
	glUniform3f(glGetUniformLocation(rendering_program, "Spot.direction"), direction[0]-5, direction[1], direction[2]-110);

	//initial
	model = mat4(1.0);

	view = LookAt(eye, at, up);
	projection = Perspective(90.0f, 1.33f, 10.0f, 1000.0f);
	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

	glUniformMatrix4fv(mv_location, 1, GL_TRUE, model*view);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	//base
	model = Translate(vec3(0.0f, -3030.0f, 0.0f))*Scale(3000.0f);
	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 1);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.1, 0.8, 0.3, 1.0));
	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), NULL_TEXTURE);
	glUniform1i(glGetUniformLocation(rendering_program, "NormalMapTex"), NULL_TEXTURE);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);

	sp->render();


	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 1);
	glUniform4fv(glGetUniformLocation(rendering_program, "orgColor"), 1, vec4(0.8, 0.8, 1.0, 1.0));


	model = Translate(80, -30, -50);
	tree1->render();

	model = Translate(0, -30, -83)*Scale(0.7);
	tree1->render();

	model = Translate(-70, -30, 0)*Scale(0.5);;
	tree1->render();

	model = Scale(1.0);

	if (season[nowseason].falling)
		snowy->render();
	glEnable(GL_DEPTH_TEST);

	//sun
	model = Translate(vec3(-40.0f, 40.0f, -100.0f))*Scale(3.0f);
	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 0);
	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), 0);
	glUniform1i(glGetUniformLocation(rendering_program, "NormalMapTex"), 10);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, Ortho(-60,60,-45,45,-10,1000));
	sp->render();

	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	//humo

	model = Translate(vec3(0.0f, -10.0f, 0.0f))*Scale(5.0f);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	me->render();

	//Dog
	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), NULL_TEXTURE);
	glUniform1i(glGetUniformLocation(rendering_program, "NormalMapTex"), NULL_TEXTURE);
	model = Translate(vec3(30.0f, -10.0f, 0.0f))*Scale(5.0f);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);

	Dog->render();

	model = Translate(-20,-30,20);
	f1->render();

	
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
			temp = mvmult(Translate(20*vec3(pos1.x - pos2.x, pos1.y - pos2.y, pos1.z - pos2.z)), vec4(eye.x, eye.y, eye.z, 1));
			eye.x = temp.x;
			eye.y = temp.y;
			eye.z = temp.z;
			temp = mvmult(Translate(20*vec3(pos1.x - pos2.x, pos1.y - pos2.y, pos1.z - pos2.z)), vec4(at.x, at.y, at.z, 1));
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
	case 'b': 
		if (autoR)
			autoR = false;
		else
			autoR = true;
		me->position = vec3(1.0);
		Dog->position = vec3(1.0);
		; break;
	case '7': 
		if (spotcont == 0)
			spotcont = 1;
		else
			spotcont = 0;
		glUniform1i(glGetUniformLocation(rendering_program, "Spot.onoff"), spotcont);
		break;
	case '8':
		if (phongcont == 0)
			phongcont = 1;
		else
			phongcont = 0;
		glUniform1i(glGetUniformLocation(rendering_program, "Spot.phong"), phongcont);
		break;

	case '`':
		if (tree1->falling){
			tree1->falling = false;
			tree1->init();
		}
		else
			tree1->falling = true;
		break;

	case '1':
		nowseason = 0;
		break;
	case '2':
		nowseason = 1;
		break;
	case '3':
		nowseason = 2;
			break;
	case '4':
		nowseason = 3;
		break;
	case '5':
		nowclimate = 0;
		break;
	case '6':
		nowclimate = 1;
		break;
	case 'n':
		if (season[nowseason].falling)
			season[nowseason].falling = false;
		else{
			season[nowseason].falling = true;
			snowy->render();
		}
		break;
	case 'm':
		me->destination = vec3(0, 0, 0);
		break;
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
	glutCreateWindow("20132390 Final Project");
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
