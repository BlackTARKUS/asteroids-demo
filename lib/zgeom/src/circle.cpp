
#include "circle.h"
#include "intersect.h"
#include "util.h"
#include <math.h>

#define PI 3.14159265

/////////////////////////////////////////////////
// Return number of points to be generated by the
// midpoint circle algorithm given radius r and
// zoom level.
/////////////////////////////////////////////////
int mp_circle_points(int r, int zoom) {
	return r*zoom*8;
}

/////////////////////////////////////////////////
// Generate a list of vertices using the midpoint circle algorithm.
// Given a radius r and zoom factor, generate a vector of
// circle points where the pixels are scaled according to zoom.
/////////////////////////////////////////////////
std::vector<vec3> midpoint_circle(vec3 c, int r, int zoom) {

	int d = r*zoom;
	std::vector<vec3> result(d*8);
	float step = 1/(float)zoom;
	float x = r - 1;
	float y = 0;
	float dx = 1;
	float dy = 1;
	float err = dx - (r << 1);
	
	for(int i=0; x >= y; i++) {
		result[d*0 + i]     = {c.x + x, c.y + y};
		result[d*1 + d-i-1] = {c.x + y, c.y + x}; // CW
		result[d*2 + i]     = {c.x - y, c.y + x};
		result[d*3 + d-i-1] = {c.x - x, c.y + y}; // CW
		result[d*4 + i]     = {c.x - x, c.y - y};
		result[d*5 + d-i-1] = {c.x - y, c.y - x}; // CW
		result[d*6 + i]     = {c.x + y, c.y - x};
		result[d*7 + d-i-1] = {c.x + x, c.y - y}; // CW
	
		if(err <= 0) {
			y += 1*step;
			err += dy;
			dy += 2*step;
		}
	
		else {
			x -= 1*step;
			dx += 2*step;
			err += dx - (r << 1);
		}
	}
	return result;
}

/////////////////////////////////////////////////
// Generate a list of vertices along a circle centered at
// point c with radius r, having d points per octant.
/////////////////////////////////////////////////
std::vector<vec3> gen_circle(vec3 c, float r, int d) {
	std::vector<vec3> result(d*8);
	float k = PI/(4.0*d);
	float x=r;
	float y=0.0;
	float c45 = cos(PI/4);
	for(int i=0; i<d; i++) {
		x = r*(float)cos(i*k);
		y = r*(float)sin(i*k);
		/*
		result[d*0 + i]     = {c.x + x, c.y + y};
		result[d*1 + d-i-1] = {c.x + y, c.y + x}; // CW
		result[d*2 + i]     = {c.x - y, c.y + x};
		result[d*3 + d-i-1] = {c.x - x, c.y + y}; // CW
		result[d*4 + i]     = {c.x - x, c.y - y};
		result[d*5 + d-i-1] = {c.x - y, c.y - x}; // CW
		result[d*6 + i]     = {c.x + y, c.y - x};
		result[d*7 + d-i-1] = {c.x + x, c.y - y}; // CW
		*/
		//////////////////////////////////////
		// Using symmetry alone creates 
		// duplicate points every 45 deg.
		// We can solve this by applying a 
		// rotation to every other octant.
		//////////////////////////////////////
		result[d*0 + i] = {c.x + x, c.y + y};
		result[d*1 + i] = {c.x + x*c45 - y*c45, c.y + x*c45 + y*c45}; // rot45
		result[d*2 + i] = {c.x - y, c.y + x};
		result[d*3 + i] = {c.x - x*c45 - y*c45, c.y + x*c45 - y*c45}; // rot135
		result[d*4 + i] = {c.x - x, c.y - y};
		result[d*5 + i] = {c.x - x*c45 + y*c45, c.y - x*c45 - y*c45}; // rot225
		result[d*6 + i] = {c.x + y, c.y - x};
		result[d*7 + i] = {c.x + x*c45 + y*c45, c.y - x*c45 + y*c45}; // rot315
	}
	return result;
}

/////////////////////////////////////////////////
// Generate a list of vertices along an arc from two points
// defining a chord of a circle. The two points form a 
// counterclockwise arc of angle theta, with each octant of
// the circle that owns the arc having d points.
// The angle theta is in degrees, being between 1 and 360.
/////////////////////////////////////////////////
std::vector<vec3> arc(vec3 p1, vec3 p2, float theta, int d) {
	std::vector<vec3> result;
	if(theta > 360 || theta < 1)
		return result;
	float angle = theta*PI/180.0;

	// vector from p1 to p2
	vec3 chord = vec3Diff(p2, p1);
	float hc = vec3Mag(chord)/2.0;

	float r = vec3Mag(chord)/(2*sin(angle/2.0));
	float h = sqrt(r*r - hc*hc);
	if((int)theta%360 > 180)
		h = -h;

	vec3 center = vec3Scale(chord, 0.5);
	vec3 height = vec3Unit(vec3Cross(center, {0.0,0.0,-1.0}));
	height = vec3Scale(height, h);
	center = vec3Sum(center, height);
	center = vec3Sum(center, p1);

	// get the circle, intersect with rays, keep the CCW arc
	std::vector<vec3> circle = gen_circle(center, r, d);
	//std::vector<vec3> circle = midpoint_circle(center, r, 1.0);
	vec3 p1ray = vec3Sum(vec3Scale(vec3Diff(p1,center), 2), center);
	vec3 p2ray = vec3Sum(vec3Scale(vec3Diff(p2,center), 2), center);
	vec3 p1v, p2v;
	int i=0, next=1;
	while(1) {
		i = positive_modulo(i, circle.size());
		next = positive_modulo(i+1, circle.size());
		if(intersect_ss(circle[i], circle[next], center, p1ray, &p1v)) {
			result.push_back(p1v);
			break;
		}
		i++;
	}
	while(1) {
		i = positive_modulo(i, circle.size());
		next = positive_modulo(i+1, circle.size());
		result.push_back(circle[i]);
		if(intersect_ss(circle[i], circle[next], center, p2ray, &p2v)) {
			result.push_back(p2v);
			break;
		}
		i++;
	}

	return result;
}

std::vector<vec3> midpoint_arc(vec3 p1, vec3 p2, float theta, float knife) {
	std::vector<vec3> result;
	if(theta > 360 || theta < 1)
		return result;
	float angle = theta*PI/180.0;

	// vector from p1 to p2
	vec3 chord = vec3Diff(p2, p1);
	float hc = vec3Mag(chord)/2.0;

	float r = vec3Mag(chord)/(2*sin(angle/2.0));
	float h = sqrt(r*r - hc*hc);
	if((int)theta%360 > 180)
		h = -h;

	vec3 center = vec3Scale(chord, 0.5);
	vec3 height = vec3Unit(vec3Cross(center, {0.0,0.0,-1.0}));
	height = vec3Scale(height, h);
	center = vec3Sum(center, height);
	center = vec3Sum(center, p1);

	// get the circle, intersect with rays, keep the CCW arc
	std::vector<vec3> circle = midpoint_circle(center, r, 1.0);

	unsigned i = 0;
	while(1) {
		i = positive_modulo(i, circle.size());
		if(circle[i].x < knife)
			break;
		i++;
	}
	while(1) {
		i = positive_modulo(i, circle.size());
		if(circle[i].x >= knife)
			break;
		i++;
	}
	while(1) {
		i = positive_modulo(i, circle.size());
		if(circle[i].x < knife)
			break;
		result.push_back(circle[i]);
		i++;
	}

	return result;
}
