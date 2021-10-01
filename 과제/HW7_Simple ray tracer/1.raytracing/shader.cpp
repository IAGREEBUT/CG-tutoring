#include "shader.h"
#include <math.h>

using namespace raytraceData;


shader::shader()
{
}

shader::~shader()
{
}

material* shader::makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb) {
	material* m;

	/* allocate memory */
	m = new material();

	/* put stuff in it */
	m->c.r = r;
	m->c.g = g;
	m->c.b = b;
	m->amb = amb;
	return(m);
}


void shader::findDirVector(point* p, point* pos, vector* lightDir) {
	lightDir->x = (pos->x - p->x) / sqrt(pow(pos->x - p->x, 2) + pow(pos->y - p->y, 2) + pow(pos->z - p->z, 2));
	lightDir->y = (pos->y - p->y) / sqrt(pow(pos->x - p->x, 2) + pow(pos->y - p->y, 2) + pow(pos->z - p->z, 2));
	lightDir->z = (pos->z - p->z) / sqrt(pow(pos->x - p->x, 2) + pow(pos->y - p->y, 2) + pow(pos->z - p->z, 2));
	lightDir->w = 0.0;
}


void shader::findhalfDirVector(point* vec1, point* vec2, vector* halfway) {
	halfway->x = (vec1->x + vec2->x) / sqrt(pow(vec1->x + vec2->x, 2) + pow(vec1->y + vec2->y, 2) + pow(vec1->z + vec2->z, 2));
	halfway->y = (vec1->y + vec2->y) / sqrt(pow(vec1->x + vec2->x, 2) + pow(vec1->y + vec2->y, 2) + pow(vec1->z + vec2->z, 2));
	halfway->z = (vec1->z + vec2->z) / sqrt(pow(vec1->x + vec2->x, 2) + pow(vec1->y + vec2->y, 2) + pow(vec1->z + vec2->z, 2));
	halfway->w = 0.0;
}

/* LIGHTING CALCULATIONS */

/* shade */
/* color of point p with normal vector n and material m returned in c */
void shader::shade(point* p, vector* n, material* m, color* c,light* l, point* viewpoint, int vis) {
	//컬러를 정해야하는 포지션, 노멀, 만나는 곳의 k값임, 

	/* so far, just finds ambient component of color */
	c->r = m->amb * m->c.r* l->amb * l->c.r;
	c->g = m->amb * m->c.g * l->amb * l->c.g;
	c->b = m->amb * m->c.b * l->amb * l->c.b;

	//specualr와 diffuse를 추가하고 light를 추가해서 계산..?

	//vector lightDir = (point*)(l->pos - p) / sqrt(pow(l->pos - p, 2));
	
	//diffuse
	vector lightDir;
	findDirVector(p, l->pos, &lightDir);

	GLfloat diff = max(lightDir.x * n->x + lightDir.y * n->y + lightDir.z * n->z , 0.0);
	c->r += diff * l->diff * m->c.r * l->c.r * vis;
	c->g += diff * l->diff * m->c.g * l->c.g * vis;
	c->b += diff * l->diff * m->c.b * l->c.b * vis;

	//specular
	

	vector viewDir;
	findDirVector(p, viewpoint, &viewDir);
	vector halfway;
	findhalfDirVector(&lightDir, &viewDir, &halfway);
	GLfloat spec = pow(max(n->x * halfway.x + n->y * halfway.y + n->z * halfway.z, 0.0),l->shiness);


	c->r += spec * l->spec * m->c.r * l->c.r * vis;
	c->g += spec * l->spec * m->c.g * l->c.g * vis;
	c->b += spec * l->spec * m->c.b * l->c.b * vis;

	/* clamp color values to 1.0 */
	if (c->r > 1.0) c->r = 1.0;
	if (c->g > 1.0) c->g = 1.0;
	if (c->b > 1.0) c->b = 1.0;

}


