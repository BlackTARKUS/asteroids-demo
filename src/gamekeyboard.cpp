
#include "gameState.h"
#include "game.h"
#include <GL/glut.h>
#include <sys/types.h> 
#include <signal.h> 
#include <iostream>

static bool paused = false;
static bool start = true;

void Game::keyDown(unsigned char key, int x, int y) {
	switch(key) {
		case 's':
		case 'S':
			if(start == true) {
				paused = !paused;
				start = !start;
			}
			break;
		case 'p':
		case 'P':		
			paused = !paused;
			if(paused) {
				displayText("Paused, Press P to continue playing");	
			}
			break;

		case 't':
		case 'T':
			if(this->tessControl == 0){
				this->tessControl = 1;				
			}
			else{
				this->tessControl = 0;
			}
			break;
		
		case 'q':
		case 'Q':
			exit(0);
			break;
		case 32: // Spacebar
			missiles.push_back(
				Missile(this->spaceship.pos, this->spaceship.theta)
			); 
			missileCount++;
			break;
		default:
			break;
	}
}

void Game::keyUp(unsigned char key, int x, int y) {
	// handle key release
}

void Game::specialKeyDown(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_UP:
            //do nothing
            break;
		case GLUT_KEY_DOWN:
			// do nothing
			break;
		case GLUT_KEY_LEFT:
			spaceship.alpha = 0.1;
			break;
		case GLUT_KEY_RIGHT:
			spaceship.alpha = -0.1;
			break;
		default:
			break;
	}
}

void Game::specialKeyUp(int key, int x, int y) {
	switch(key) {
		case GLUT_KEY_LEFT:
			spaceship.alpha = 0.0;
			break;
		case GLUT_KEY_RIGHT:
			spaceship.alpha = 0.0;
			break;
		default:
			break;
	}
}
// Returns if the game is paused
bool getPaused() {
	return paused;
}
// Returns if the game is in the starting state
bool getStart() {
	return start;
}

void setPaused(int value){
    paused = value;
}

void setStart(int value){
    start = value;
}