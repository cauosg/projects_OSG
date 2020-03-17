#include "diging.h"
#include "bmpreader.h"
#include "vboplane.h"

void startup();
void render();
void shutdown();
extern void version();
GLuint compile_shaders(GLchar* vsource, GLchar* fsource);

GLuint rendering_program;

float angle, t;

GLint mv_location;
GLint proj_location;

GLuint tex_object[2];

const int WIDTH = 800; const int HEIGHT = 600;
VBOPlane* plane;
GLuint initVel, startTime, particles; 
GLuint nParticles; 

inline float randFloat() {return ((float)rand()/RAND_MAX);}
inline float mix(float x, float y, float a) { return ((1.0-a)*x + a*y); }  
void initBuffers(); 
void startup()
{
	/******* OpenGL Initialization */

	t = 0.0;
	/**** Shaders as variables */
	rendering_program = compile_shaders("particles_vs.glsl", "particles_fs.glsl");
	glUseProgram(rendering_program);

	// modeling
   	plane = new VBOPlane(13.0f, 10.0f, 200, 2);
	
	// matrices
	mat4 model = mat4(1.0); angle = (float)M_PI/2.0; t = 0.0f;
	mat4 view = LookAt(vec3(3.0f, 1.5f, 3.0f), vec3(0.0f,1.5f,0.0f), vec3(0.0f,1.0f,0.0f));
	mat4 projection = Perspective(60.0f, (float)WIDTH/HEIGHT, 0.3f, 100.0f);

	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");

	glUniformMatrix4fv(mv_location,1,GL_TRUE, view*model);
	glUniformMatrix4fv(proj_location,1,GL_TRUE, projection);

	// Generate our vertex buffers
	initBuffers();

	// the particle texture
	glActiveTexture(GL_TEXTURE0);
	BMPReader::loadTex("fire.bmp");
	glUniform1i(glGetUniformLocation(rendering_program, "ParticleTex"), 0);
}

void initBuffers() 
{ 
    nParticles = 8000; 

    // Generate the buffers 
    glGenBuffers(1, &initVel);   // Initial velocity buffer 
    glGenBuffers(1, &startTime); // Start time buffer 

    // Allocate space for all buffers 
    int size = nParticles * 3 * sizeof(float); 
    glBindBuffer(GL_ARRAY_BUFFER, initVel); 
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW); 
    glBindBuffer(GL_ARRAY_BUFFER, startTime); 
    glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW); 

    // Fill the first velocity buffer with random velocities 
    vec3 v(0.0f); 
    float velocity, theta, phi; 
    GLfloat *data = new GLfloat[nParticles * 3]; 
    for (unsigned int i = 0; i < nParticles; i++ ) { 
        theta = mix(0.0f, (float)M_PI / 6.0f, randFloat()); 
        phi = mix(0.0f, (float)2*M_PI, randFloat()); 
        v.y = sinf(theta) * cosf(phi)-1.0f; 
        v.x = cosf(theta)-1.0f; 
        v.z = sinf(theta) * sinf(phi); 
        velocity = mix(-2.25f,2.5f,randFloat()); 
        v = normalize(v) * velocity; 

        data[3*i]   = v.x; 
        data[3*i+1] = v.y; 
        data[3*i+2] = v.z; 
    } 
    glBindBuffer(GL_ARRAY_BUFFER,initVel); 
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data); 
	delete [] data; 

    // Fill the start time buffer 
    float *data1 = new GLfloat[nParticles]; 
    float time = 0.0f; 
    float rate = -0.0075f; 
    for( unsigned int i = 0; i < nParticles; i++ ) { 
        data1[i] = time; 
        time += rate; 
    } 
    glBindBuffer(GL_ARRAY_BUFFER,startTime); 
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data1); 
	glBindBuffer(GL_ARRAY_BUFFER,0); 
    delete [] data1; 

    // Attach these to the torus's vertex array 
    glGenVertexArrays(1, &particles); 
	glBindVertexArray(particles); 

    glBindBuffer(GL_ARRAY_BUFFER, initVel); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); 
    glEnableVertexAttribArray(0); 

    glBindBuffer(GL_ARRAY_BUFFER, startTime); 
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL); 
    glEnableVertexAttribArray(1); 
} 

void idle(){ 
	t += 0.002f;
	glutPostRedisplay();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.6f, 0.8f, 1.0f, 0.0f); 
	static mat4 model(1.0);
	mat4 view = LookAt(vec3(0.0f, 0.0f, 9.5f),
		vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f)
		);
	mat4 projection = Perspective(90.0f, 1.33f, 10.0f, 1000.0f);

	mv_location = glGetUniformLocation(rendering_program, "mv_matrix");
	proj_location = glGetUniformLocation(rendering_program, "proj_matrix");
	model = RotateZ(-90.0f);//Rotate(90.0f, vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, model*view);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);

	glUseProgram(rendering_program);

	glUniform1f(glGetUniformLocation(rendering_program, "Time"), t);

	// Disable depth test
	glDisable(GL_DEPTH_TEST);
	// Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable Point Size. Enable Point Coord.
	glEnable(GL_PROGRAM_POINT_SIZE);glPointSize(10.0f); 
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

	glDrawArrays(GL_POINTS, 0, nParticles);

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
    glutCreateWindow( "Particle Fountains" );
	glewInit();
    
	version();
	startup();
	glutDisplayFunc(render); 
    glutIdleFunc(idle);

    glutMainLoop();
	shutdown();
    return 0;
}
