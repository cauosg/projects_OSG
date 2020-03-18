#ifndef VBOSPHERE_H
#define VBOSPHERE_H

#include "diging.h"

class VBOSphere
{
private:
	unsigned int vaoHandle;
	GLuint nVerts, elements;
	float radius;
	GLuint slices, stacks;

	void generateVerts(float *, float *, float *, GLuint *);

public:
	VBOSphere(float, GLuint, GLuint);

	void render() const;

	int getVertexArrayHandle();
};

#endif // VBOSPHERE_H