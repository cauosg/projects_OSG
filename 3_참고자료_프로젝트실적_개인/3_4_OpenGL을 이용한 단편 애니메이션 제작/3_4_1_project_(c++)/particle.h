#ifndef PARTICLE_H
#define PARTICLE_H

class Particle{
public:
	Particle();
	void startup();
	void render();
	void initBuffers();
	void initBuffers2();
	void zerot();
	float t;
};
#endif