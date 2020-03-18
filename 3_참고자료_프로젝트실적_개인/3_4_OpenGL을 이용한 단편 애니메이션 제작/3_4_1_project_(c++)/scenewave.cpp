#include "diging.h"
#include "vboplane.h"

void startup();
void render();
void shutdown();
extern void version();
extern GLuint compile_shaders(GLchar* vsource, GLchar* fsource);

GLuint rendering_program;

float angle, t;

GLint mv_location;
GLint proj_location;

GLuint tex_object[2];

const int WIDTH = 800; const int HEIGHT = 600;
VBOPlane* plane;

void startup()
{
	/******* OpenGL Initialization */
	//glEnable(GL_CULL_FACE); glFrontFace(GL_CCW); //  polygons with clockwise winding are front-facing
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);

	/**** Shaders as variables */
	rendering_program = compile_shaders("wave_vs.glsl", "wave_fs.glsl");
	glUseProgram(rendering_program);

	// modeling
   	plane = new VBOPlane(13.0f, 10.0f, 200, 2);
	
	// matrices
	mat4 model = mat4(1.0); angle = (float)M_PI/2.0; t = 0.0f;
	mat4 view = LookAt(vec3(10.0f * cos(angle), 4.0f, 10.0f * sin(angle)), vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
	mat4 projection = Perspective(60.0f, (float)WIDTH/HEIGHT, 0.3f, 100.0f);

	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

	glUniformMatrix4fv(mv_location,1,GL_TRUE, model*view);
	glUniformMatrix4fv(proj_location,1,GL_TRUE, projection);

	// lighting
	glUniform3fv(glGetUniformLocation(rendering_program, "Light.Intensity"), 1, vec3(0.9, 0.9, 0.9));
	glUniform4fv(glGetUniformLocation(rendering_program, "Light.Position"), 1,
		view*vec4(5.0, 3.0, 5.0, 1.0));
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Kd"), 0.9f, 0.9f, 0.9f);
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ks"), 0.8f, 0.8f, 0.8f);
	glUniform3f(glGetUniformLocation(rendering_program, "Material.Ka"), 0.2f, 0.2f, 0.2f);
	glUniform1f(glGetUniformLocation(rendering_program, "Material.Shininess"), 100.0f);

	glUniform1f(glGetUniformLocation(rendering_program, "Time"), t);
}

void idle(){ 
	t += 0.001f;
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.7f, 0.7f, 0.7f, 1.0f); 

	glUseProgram(rendering_program);
	mat4 model = mat4(1.0);
	mat4 view = LookAt(vec3(10.0f * cos(angle),4.0f,10.0f * sin(angle)), vec3(0.0f,0.0f,0.0f), vec3(0.0f,1.0f,0.0f));
	
	glUniformMatrix4fv(mv_location,1,GL_TRUE, model*view);
	glUniform1f(glGetUniformLocation(rendering_program, "Time"), t);

	plane->render();

	glutSwapBuffers();
}

void shutdown()
{
	glDeleteProgram(rendering_program);
}

int main( int argc, char **argv )
{
    glutInit( &argc, argv );
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow( "Displacement Waves" );
	glewInit();
    
	version();
	startup();
	glutDisplayFunc(render); 
    glutIdleFunc(idle);

    glutMainLoop();
	shutdown();
    return 0;
}
