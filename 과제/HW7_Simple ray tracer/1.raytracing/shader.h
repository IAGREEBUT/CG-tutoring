#pragma once

#include <glad/glad.h>
#include <stdlib.h> 
#include "raytraceData.h"

class shader {
public: 
	shader();
	~shader();

	raytraceData::material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb);


	
	void shade(raytraceData::point* p, raytraceData::vector* n, raytraceData::material* m, raytraceData::color* c, raytraceData::light* l, raytraceData::point* viewpoint, int vis);
	void shader::findDirVector(raytraceData::point* p, raytraceData::point* pos, raytraceData::vector* lightDir);
	void shader::findhalfDirVector(raytraceData::point* vec1, raytraceData::point* vec2, raytraceData::vector* halfway);
};
