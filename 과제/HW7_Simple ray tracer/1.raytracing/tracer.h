#pragma once

#include "raytraceData.h"

class tracer {
public:
	tracer();
	~tracer();

	void findPointOnRay(raytraceData::ray* r, float t, raytraceData::point* p);
	int raySphereIntersect(raytraceData::ray* r, raytraceData::sphere* s, float* t);
	int rayPlaneIntersect(raytraceData::ray* r, raytraceData::plane* p1, float* t);
	void findSphereNormal(raytraceData::sphere* s, raytraceData::point* p, raytraceData::vector* n);
	void findPlaneNormal(raytraceData::plane* p1, raytraceData::vector* n);
	void trace(raytraceData::ray* r, raytraceData::point* p, raytraceData::vector* n, raytraceData::material** m);
	int tracer::vis(raytraceData::ray* r);
	void vectorCross(raytraceData::vector* vec1, raytraceData::vector* vec2, raytraceData::vector* result);
	void findDirVector(raytraceData::point* a, raytraceData::point* b, raytraceData::vector* dir);

	raytraceData::sphere* s1;	   /* the scene: so far, just one sphere */
	raytraceData::light* l;
	raytraceData::plane* p1;
};