/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include "includes.h"
#include "framework.h"
#include "image.h"

struct Particle {
	int x;
	int y;
};

class Application
{
public:
	//window
	SDL_Window* window;
	float window_width;
	float window_height;

	Image framebuffer;

	float time;

	//keyboard state
	const Uint8* keystate;

	//mouse state
	int mouse_state; //tells which buttons are pressed
	Vector2 mouse_position; //last mouse position
	Vector2 mouse_delta; //mouse movement in the last frame

	int exercise;
	Image image;
	Image bola;
	Image canvas;

	bool draw;
	Color color;

	Particle particles[200];

	//constructor
	Application(const char* caption, int width, int height);

	//main methods
	void init(void);
	void render(Image& framebuffer);
	void update(double dt);

	//methods for events
	void onKeyDown(SDL_KeyboardEvent event);
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown(SDL_MouseButtonEvent event);
	void onMouseButtonUp(SDL_MouseButtonEvent event);

	//other methods to control the app
	void setWindowSize(int width, int height) {
		glViewport(0, 0, width, height);
		this->window_width = width;
		this->window_height = height;
		framebuffer.resize(width, height);
	}

	Vector2 getWindowSize()
	{
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		return Vector2(w, h);
	}

	void start();
	void exercise1();
	void tresholdFilter();
	void invertFilter();
	void rotateImage(float angle);
	void scaleImage();
	void particleAnimation();
	void exercise6();
	void paint();

};


#endif 
