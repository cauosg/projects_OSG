#ifndef BASE_H
#define BASE_H

#include "diging.h"
#include "bmpreader.h"
#include "vbomesh.h"
#include "vbosphere.h"
#include "runningDog.h"
#include "particle.h"
#include "VBOplane.h"
#include "humo.h"
#include "flower.h"
#include "tree.h"
#include "snow.h"
#include "diging.h"
class _climate{
public:
	vec3 Ka;
	vec3 Ks;
	vec3 Kd;
};
class _season{
public:
	vec3 leafcolor;

	int tex_num = 21;
	bool falling;
	vec3 tex_color;
	float fallingV;
};
#endif // BASE_H