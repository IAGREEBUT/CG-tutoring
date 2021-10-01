#include "tracer.h"

#include <math.h>

using namespace raytraceData;

tracer::tracer() :
	s1(NULL),l(NULL),p1(NULL)
{
}



tracer::~tracer()
{
}

/* point on ray r parameterized by t is returned in p */
void tracer::findPointOnRay(ray* r, float t, point* p) {
	p->x = r->start->x + t * r->end->x;
	p->y = r->start->y + t * r->end->y;
	p->z = r->start->z + t * r->end->z;
	p->w = 1.0;
}

/* raySphereIntersect */
/* returns TRUE if ray r hits sphere s, with parameter value in t */
int tracer::raySphereIntersect(ray* r, sphere* s, float* t) {
	point p;   /* start of transformed ray */
	float a, b, c;  /* coefficients of quadratic equation */
	float D;    /* discriminant 판별*/
	point* v;

	/* transform ray so that sphere center is at origin */
	/* don't use matrix, just translate! */
	p.x = r->start->x - s->c->x;
	p.y = r->start->y - s->c->y;
	p.z = r->start->z - s->c->z;
	v = r->end; /* point to direction vector */


	a = v->x * v->x + v->y * v->y + v->z * v->z;
	b = 2 * (v->x * p.x + v->y * p.y + v->z * p.z);
	c = p.x * p.x + p.y * p.y + p.z * p.z - s->r * s->r;

	D = b * b - 4 * a * c; //근의 공식 판별식 부분 

	if (D < 0) {  /* no intersection */
		return (FALSE);
	}
	else {
		D = static_cast<float>(sqrt(D));
		/* First check the root with the lower value of t: */
		/* this one, since D is positive */
		*t = (-b - D) / (2 * a);
		/* ignore roots which are less than zero (behind viewpoint) */
		if (*t < 0) {
			*t = (-b + D) / (2 * a); //근의공식임 
		}
		if (*t < 0) { return(FALSE); }
		else return(TRUE);
	}
}

int tracer::rayPlaneIntersect(ray* r, plane* p1, float* t) {
	// NP + D = 0 
	vector n;
	float d;
	point* v;

	findPlaneNormal(p1, &n);
	v = r->end; /* point to direction vector  P = P0 + su에서 u임 */ 

	d=-(n.x * p1->top_left->x + n.y * p1->top_left->y + n.z * p1->top_left->z);

	float parallel = v->x * n.x + v->y * n.y + v->z * n.z;//0이면 평행

	if (parallel == 0) {
		return(FALSE);
	}
	else {
		*t = -(d + n.x * r->start->x + n.y * r->start->y + n.z * r->start->z) / parallel;

		if (*t < 0) {
			return (FALSE);
		}
		else {
			return (TRUE);
		}
	}


}


int tracer::vis(ray* r) {

	float t = 0;     /* parameter value at first hit */
	float t2 = 0;
	int hit1 = FALSE;
	int hit2 = FALSE;
	int res = TRUE; // no intersect


	hit1 = raySphereIntersect(r, s1, &t);
	hit2 = rayPlaneIntersect(r, p1, &t2);

	if (hit1 == TRUE) {
		res = FALSE; // intersect 발생 
	}
	if (hit2 == TRUE) {
		res = FALSE; // intersect 발생 
	}


	return(res);

}


/* normal vector of s at p is returned in n */
/* note: dividing by radius normalizes */
void tracer::findSphereNormal(sphere* s, point* p, vector* n) {
	n->x = (p->x - s->c->x) / s->r;
	n->y = (p->y - s->c->y) / s->r;
	n->z = (p->z - s->c->z) / s->r;
	n->w = 0.0;
}


void tracer::findPlaneNormal(plane* p1, vector* n) {
	vector ab;
	vector ac;

	findDirVector(p1->top_left, p1->top_right, &ab);
	findDirVector(p1->bottom_right, p1->top_right, &ac);
	vectorCross(&ab, &ac, n);

}


void tracer::findDirVector(point* a, point* b, vector* dir) {
	dir->x = (a->x - b->x);
	dir->y = (a->y - b->y);
	dir->z = (a->z - b->z);
	dir->w = 0.0;
}

void tracer::vectorCross(vector* vec1, vector* vec2, vector* result) {
	result->x = vec1->y * vec2->z - vec1->z * vec2->y;
	result->y = vec1->z * vec2->x - vec1->x * vec2->z;
	result->z = vec1->x * vec2->y - vec1->y * vec2->x;
	result->w = 0.0;

	result->x = result->x / sqrt(pow(result->x, 2) + pow(result->y, 2) + pow(result->z, 2));
	result->y = result->y / sqrt(pow(result->x, 2) + pow(result->y, 2) + pow(result->z, 2));
	result->z = result->z / sqrt(pow(result->x, 2) + pow(result->y, 2) + pow(result->z, 2));
}


/* trace */
/* If something is hit, returns the finite intersection point p,
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
void tracer::trace(ray* r, point* p, vector* n, material** m) {
	float t = 0;     /* parameter value at first hit */
	float t2 = 0;
	int hit = FALSE;
	int hit2 = FALSE;

	//ray와 구가 만나는지, 어떤 점에서 만나는지(지금 화면에 구밖에없어서..)
	//다른 물체만들면 다른 인터섹트를 만들어야함
	hit = raySphereIntersect(r, s1, &t);
	hit2 = rayPlaneIntersect(r, p1, &t2);
	if (hit) {
		*m = s1->m;
		findPointOnRay(r, t, p); //만나는곳의 포지션
		findSphereNormal(s1, p, n); //만나는 곳의 노멀 
	}
	else if (hit2) {
		*m = p1->m;
		findPointOnRay(r, t2, p); //만나는곳의 포지션 -> t이거 s임... P = P0 + su의 걔..
		findPlaneNormal(p1, n); //만나는 곳의 노멀 
	}
	else {
		/* indicates nothing was hit */
		p->w = 0.0;
	}
}