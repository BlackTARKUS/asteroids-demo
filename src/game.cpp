
#include "game.h"
#include "asteroid.h"
#include "bust.h"
#include "zgeom.h"
#include "glFuncs.h" // for rendering clipWindow
#include <vector>
#include <cmath>
#include <stdio.h>

Game::Game() {

}

void Game::init(int window_width, int window_height, float octRadius) {

	this->origin = { window_width/2.0f, window_height/2.0f };

	// generate some asteroids
	// TODO
	// make placement random
	float x=50.0, y=500.0;
	for(int i=0; i<2; i++) {
		Asteroid a( (vec3) {this->origin.x + x, y} , 25.0 );
		Asteroid b( (vec3) {this->origin.x - x, y} , 25.0 );
		this->asteroids.push_back(a);
		this->asteroids.push_back(b);
		x += 100.0;
	}

	// create the clipping window
	float PI = 3.14159265359;
	float ang = PI/8.0;
	for(int i=0; i<8; i++) {
		this->clipWindow.insert(this->clipWindow.begin(), {
			this->origin.x + (float)cos(ang)*octRadius,
			this->origin.y + (float)sin(ang)*octRadius
		});
		ang += PI/4.0;
	}
}

void Game::update() {
	// update each asteroid
	for(unsigned i=0; i<this->asteroids.size(); i++) {
		this->asteroids[i].update();
	}
	// update each triroid
	for(unsigned i=0; i<this->triroids.size(); i++) {
		this->triroids[i].update();
	}
}

void Game::render() {

	clearScreen();

	// draw the clipping window
	glBegin(GL_LINE_LOOP);
	for(unsigned i=0; i < this->clipWindow.size(); i++) {
		glVertex3f(this->clipWindow[i].x, this->clipWindow[i].y, 0.0);
	}
	glEnd();

	// draw the asteroids
	for(unsigned i=0; i < this->asteroids.size(); i++) {
		int clip = this->checkClipping( this->asteroids[i] );
		if(clip == -1) {
			asteroids[i].pos = 2*origin - asteroids[i].pos;
			asteroids[i].update();
		}
		asteroids[i].clip(this->clipWindow);
		this->asteroids[i].render();
	}
	// draw the triroids
	for(unsigned i=0; i < this->triroids.size(); i++) {
		int clip = this->checkClipping( this->triroids[i] );
		if(clip == -1) {
			triroids[i].pos = 2*origin - triroids[i].pos;
			triroids[i].update();
		}
		triroids[i].clip(this->clipWindow);
		this->triroids[i].render();
	}
	
	swapBuffers();
}

void Game::bustTest() {
	if(asteroids.size()<1)
		return;
	std::vector<Asteroid> bustRoids = bustTris(asteroids[0]);
	this->triroids.insert(this->triroids.end(), bustRoids.begin(), bustRoids.end());
	this->asteroids.erase(this->asteroids.begin());
}

int Game::checkClipping(Asteroid A) {
	unsigned outside = 0;
	for(unsigned i=0; i<A.Tverts.size(); i++) {
		if(!point_in_poly(A.Tverts[i], this->clipWindow)) {
			outside += 1;
		}
	}
	if(outside == A.Tverts.size())
		return -1; // completely outside
	return outside; // 0 if fully inside
}

