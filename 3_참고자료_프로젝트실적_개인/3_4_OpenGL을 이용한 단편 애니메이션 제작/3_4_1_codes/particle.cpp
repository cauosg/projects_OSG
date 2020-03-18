#include "diging.h"
#include "bmpreader.h"
#include "Particle.h"


extern void version();
extern GLuint compile_shaders(GLchar* vsource, GLchar* fsource);
extern vec3 eye;
extern GLuint rendering_program;
extern mat4 model;
extern mat4 view;
extern mat4 projection;
float P_angle;

extern GLint mv_location;
extern GLint proj_location;

GLuint initVel, startTime, particles, pPos;
GLuint nParticles;
inline float randFloat() { return ((float)rand() / RAND_MAX); }
inline float mix(float x, float y, float a) { return ((1.0 - a)*x + a*y); }

Particle::Particle(){
	initBuffers();
	startup();
}

void Particle::zerot(){
	t = 0;
}

void Particle::startup()
{
	/******* OpenGL Initialization */
	P_angle = (float)M_PI / 2.0; t = 0.0f;
	// matrices
	// Generate our vertex buffers
	// the particle texture

}

void Particle::initBuffers2(){
	nParticles = 8000;

	int size = nParticles * 3 * sizeof(float);
	GLfloat *data = new GLfloat[nParticles * 3];

	glGenBuffers(1, &pPos);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);


}

void Particle::initBuffers()
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
	for (unsigned int i = 0; i < nParticles; i++) {
		theta = mix(0.0f, (float)M_PI / 6.0f, randFloat());
		phi = mix(0.0f, (float)2 * M_PI, randFloat());
		v.y = sinf(theta) * cosf(phi) - 100.0f;
		v.x = cosf(theta) - 100.0f;
		v.z = sinf(theta) * sinf(phi);
		velocity = mix(-60.25f, 60.5f, 100 * randFloat())/eye.z;
		v = velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
		//srand(randFloat());
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	delete[] data;

	// Fill the start time buffer 
	float *data1 = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data1[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data1;

	// Attach these to the torus's vertex array 
	glGenVertexArrays(1, &particles);
	glBindVertexArray(particles);

	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
}

void Particle::render()
{

	glUniform1i(glGetUniformLocation(rendering_program, "coloring"), 2);

	glUniform1i(glGetUniformLocation(rendering_program, "option"), 1);
	glUniformMatrix4fv(mv_location, 1, GL_TRUE, view*model);
	glUniformMatrix4fv(proj_location, 1, GL_TRUE, projection);
	glUniform1f(glGetUniformLocation(rendering_program, "StartTime"), 0.0f);
	glUniform1f(glGetUniformLocation(rendering_program, "Time"), t);
	glUniform1i(glGetUniformLocation(rendering_program, "ColorTex"), 20);

	// Disable depth test
	glDisable(GL_DEPTH_TEST);
	// Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Enable Point Size. Enable Point Coord.
	glEnable(GL_PROGRAM_POINT_SIZE); glPointSize(20.0f/eye.z*50);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

	glBindVertexArray(particles);
	glDrawArrays(GL_POINTS, 0, 2000);
	//glDrawArrays(GL_POINTS, 1, nParticles);



	glUniform1i(glGetUniformLocation(rendering_program, "option"), 0);

	static float temp = 0;
	//temp += 0.02f;
	//t = 0.15 - 0.15f*cosf(temp);
	t += 0.005;

}
