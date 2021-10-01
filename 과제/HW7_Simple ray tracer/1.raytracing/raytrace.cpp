#include "raytrace.h"
#include <math.h>


using namespace raytraceData;

raytrace::raytrace(int w, int h)
{
	/* low-level graphics setup */
	lowlevel.initCanvas(w, h);

	/* raytracer setup */
	initCamera(w, h);
	initScene();
}

raytrace::~raytrace()
{
	if (s1 != NULL) delete s1;
	if (p1 != NULL) delete p1;
	if (l != NULL) delete l;
	if (viewpoint != NULL) delete viewpoint;
}

GLubyte* raytrace::display(void)
{	
	drawScene();  /* draws the picture in the canvas */

	return lowlevel.flushCanvas();  /* draw the canvas to the OpenGL window */	
}

void raytrace::initScene()
{

	s1 = makeSphere(0.0, 0.0, -2.0, 0.25);
	s1->m = shader.makeMaterial(0.8, 0.1, 0.15, 0.3);

	p1 = makePlane(-2.0, 0.25, -4.0);
	p1->m = shader.makeMaterial(1.0, 1.0, 1.0, 0.3);

	l = makeLight(1.0, 1.0, 1.0, -2.0, -1.0, 0.0, 1.0, 0.5, 0.8, 1.0, 32.0);

	tracer.l = l;
	tracer.s1 = s1;
	tracer.p1 = p1;

}

void raytrace::initCamera(int w, int h)
{
	viewpoint = makePoint(0.0, 0.0, 0.0, 1.0);
	pnear = -1.0;
	fovx = M_PI / 6;
	/* window dimensions */
	width = w;
	height = h;
}

void raytrace::drawScene(void)
{
	int i, j;
	GLfloat imageWidth;

	/* declare data structures on stack to avoid malloc & free */
	point worldPix;  /* current pixel in world coordinates */
	point direction;
	ray r;
	color c;

	/* z and w are constant for all pixels */
	worldPix.w = 1.0;
	worldPix.z = pnear;

	//모든 픽셀에 대하여 레이를 계산 
	r.start = &worldPix;
	r.end = &direction;

	imageWidth = 2 * pnear * tan(fovx / 2);

	/* trace a ray for every pixel */
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {

			/* find position of pixel in world coordinates */
			/* y position = (pixel height/middle) scaled to world coords */
			worldPix.y = (j - (height / 2)) * imageWidth / width;
			/* x position = (pixel width/middle) scaled to world coords */
			worldPix.x = (i - (width / 2)) * imageWidth / width;

			/* find direction */
			/* note: direction vector is NOT NORMALIZED */
			calculateDirection(viewpoint, &worldPix, &direction);//레이의 방향결정

			/* Find color for pixel */
			rayColor(&r, &c); //레이를 쏨 -> 만나는 컬러값을c에저장
			/* write the pixel! */
			lowlevel.drawPixel(i, j, c.r, c.g, c.b); //c를 읽어와서 디스플레이
		}
	}
}

point* raytrace::makePoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	point* p;
	/* allocate memory */	
	p = new point();
	/* put stuff in it */
	p->x = x; p->y = y; p->z = z; p->w = w;
	return (p);
}

sphere* raytrace::makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {
	sphere* s;
	/* allocate memory */
	s = new sphere();

	/* put stuff in it */
	s->c = makePoint(x, y, z, 1.0);   /* center */
	s->r = r;   /* radius */
	s->m = NULL;   /* material */
	return(s);
}

plane* raytrace::makePlane(GLfloat x, GLfloat y, GLfloat z) {
	plane* p;
	/* allocate memory */
	p = new plane();

	/* put stuff in it */
	p->top_left = makePoint(x+4, y, z, 1.0);   /* center */
	p->top_right = makePoint(x, y, z, 1.0);;   /* center */
	p->bottom_left = makePoint(x+4, y, z+4, 1.0);   /* center */
	p->bottom_right = makePoint(x, y, z+4, 1.0);   /* center */

	p->m = NULL;   /* material */
	return(p);
}


light* raytrace::makeLight(GLfloat r, GLfloat g, GLfloat b, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLfloat amb, GLfloat diff, GLfloat spec, GLfloat shiness) {
	light* l;
	/* allocate memory */
	l = new light();

	/* put stuff in it */

	//light color
	l->c.r = r;
	l->c.g = g;
	l->c.b = b;

	//light pos
	l->pos = makePoint(x, y, z, 1.0);



	//light a,d,s
	l->amb = amb;
	l->diff = diff;
	l->spec = spec;
	l->shiness = shiness;
	return(l);
}


/* returns the color seen by ray r in parameter c */

//실제 수정해야하는 부분 
void raytrace::rayColor(ray* r, color* c) {
	point p;  /* first intersection point */
	vector n;
	material* m;

	p.w = 0.0;  /* inialize to "no intersection" */
	tracer.trace(r, &p, &n, &m); //레이가 날아간 곳에 어떤 물체와 만나고 그 normal이 뭔지

	//shadowray
	point lightsource;
	point direction;
	ray shadowray;

	lightsource = *l->pos;
	shadowray.start = &lightsource;
	shadowray.end = &direction;

	calculateDirection(&p, &lightsource, &direction);

	int vis = 1;
	vis = tracer.vis(&shadowray);


	if (p.w != 0.0) {
		shader.shade(&p, &n, m, c, l, viewpoint, vis);  /* do the lighting calculations */
		//여기서 로컬 일루미네이션
	}
	else {             /* nothing was hit */
		c->r = 0.0;
		c->g = 0.0;
		c->b = 0.0;
	}
}

/* vector from point p to point q is returned in v */
void raytrace::calculateDirection(point* p, point* q, point* v) {
	float length;

	v->x = q->x - p->x;
	v->y = q->y - p->y;
	v->z = q->z - p->z;
	/* a direction is a point at infinity */
	v->w = 0.0;
}
