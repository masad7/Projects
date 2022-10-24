#include "application.h"
#include "utils.h"
#include "image.h"

#include <math.h>

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w, h;
	SDL_GetWindowSize(window, &w, &h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);

	framebuffer.resize(w, h);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;

	//here add your init stuff
	image.loadTGA("../res/image.tga");
	image.scale(window_width, window_height);

	bola.loadTGA("../res/bola.tga");
	bola.scale(window_width, window_height);

	canvas.loadTGA("../res/canvas.tga");
	canvas.scale(window_width, window_height);
}

//render one frame
void Application::render(Image& framebuffer)
{
	//clear framebuffer if we want to start from scratch
	framebuffer.fill(Color::BLACK);

	//CRIDA DE TOTES LES FUNCIONS
	if (exercise == 1) { //dibuixa figures
		Application::exercise1();
	}
	else if (exercise == 2) { //dibuixa gradient blau/vermell
		Application::framebuffer.gradient(Color::BLUE, Color::RED);
	}
	else if (exercise == 3) {  //dibuixa gradient punt
		Application::framebuffer.gradientPoint();
	}
	else if (exercise == 4) { //filtre 1: treshold
		Application::tresholdFilter();
	}
	else if (exercise == 5) { //filtre 2: invert
		Application::invertFilter();
	}
	else if (exercise == 6) { //rotate the image pi/4 rad
		Application::rotateImage(PI / 4);
	}
	else if (exercise == 7) { //rotate the image 3pi/2 rad
		Application::rotateImage(3 * PI / 2);
	}
	else if (exercise == 8) {   //scale the image
		Application::scaleImage();
	}
	else if (exercise == 9) { //paint
		Application::exercise6();
		Application::paint();
	}
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (exercise == 7) //if key space is pressed
	{
		for (int idx = 0; idx < 200; ++idx) {
			particles[idx].y = (int)(particles[idx].y - seconds_elapsed * 20);
		}
	}
	//to read mouse position use mouse_position
}

//keyboard press event 
void Application::onKeyDown(SDL_KeyboardEvent event)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
	case SDL_SCANCODE_ESCAPE:
		exit(0);
		break; //ESC key, kill the app
	case SDL_SCANCODE_1:
		Application::exercise = 1;  //tecla 1 exercici 1
		break;
	case SDL_SCANCODE_2:
		Application::exercise = 2;  //tecla 2 gradient blau/vermell
		break;
	case SDL_SCANCODE_3:
		Application::exercise = 3;  //tecla 3 gradient punt
		break;
	case SDL_SCANCODE_4:
		Application::exercise = 4;  //tecla 4 filtre 1: treshold
		break;
	case SDL_SCANCODE_5:
		Application::exercise = 5;  //tecla 5 filtre2: invert
		break;
	case SDL_SCANCODE_6:
		Application::exercise = 6;  //tecla 6 rotate image pi/4 rad
		break;
	case SDL_SCANCODE_7:
		Application::exercise = 7;  //tecla 6 roate image 3pi/2 rad
		break;
	case SDL_SCANCODE_8:
		Application::exercise = 8;  //tecla 7 scale image
		break;
	case SDL_SCANCODE_9:
		Application::exercise = 9;  //tecla 9 paint
		break;
	}
}

//keyboard key up event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//...
}

//mouse button event
void Application::onMouseButtonDown(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		draw = true;
	}
}

void Application::onMouseButtonUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{
		draw = false;
	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}

//funció que ens mostren les figures (task 1)
void Application::exercise1() {
	framebuffer.drawRectangle(220, 300, 20, 150, Color::RED, true); //lletra U
	framebuffer.drawRectangle(220, 300, 75, 20, Color::RED, false);
	framebuffer.drawRectangle(295, 300, 20, 150, Color::RED, false);

	framebuffer.drawRectangle(340, 300, 20, 150, Color::RED, true); //lletra P
	framebuffer.drawRectangle(360, 430, 75, 20, Color::RED, false);
	framebuffer.drawRectangle(360, 340, 75, 20, Color::RED, false);
	framebuffer.drawRectangle(415, 360, 20, 70.5, Color::RED, false);

	framebuffer.drawRectangle(450, 300, 20, 150, Color::RED, true); //lletra F
	framebuffer.drawRectangle(460, 430, 75, 20, Color::RED, false);
	framebuffer.drawRectangle(460, 360, 50, 20, Color::RED, false);

	framebuffer.drawCircle(525, 300, 10, Color::WHITE, true); //Creem el punt final

	framebuffer.drawLine(200, 280, 565, 280, Color::WHITE);

}

//filtre treshold de la imatge que em carregat
void Application::tresholdFilter() {

	for (unsigned int idx = 0; idx < window_width; ++idx)
	{
		for (unsigned int idy = 0; idy < window_height; ++idy)
		{
			Color c = image.getPixelSafe(idx, idy); //Agafem els píxels de color de la imatge
			int red = c.r;
			int green = c.g;
			int blue = c.b;

			int median = (red + green + blue) / 3;

			if (median > 128) {
				red = 255;
				green = 255;
				blue = 255;
			}
			else {
				red = 0;
				green = 0;
				blue = 0;
			}

			framebuffer.setPixel(idx, idy, Color(red, green, blue)); //apliquem els nous colors al framebuffer
		}
	}
}

//filtre que fa els colors negatius de la imatge
void Application::invertFilter() {
	for (unsigned int idx = 0; idx < window_width; ++idx) {
		for (unsigned int idy = 0; idy < window_height; ++idy) {
			Color c = image.getPixelSafe(idx, idy);

			//trobem els colors invertits
			int red = 255 - c.r;
			int green = 255 - c.g;
			int blue = 255 - c.b;

			framebuffer.setPixel(idx, idy, Color(red, green, blue)); //apliquem els nous colors al framebuffer per a que ens mostri la nova imatge
		}
	}
}


//funció que donat un angle rota la imatge
void Application::rotateImage(float angle) {
	float x_center = window_width / 2;
	float y_center = window_height / 2;

	for (int idx = 0; idx < window_width; ++idx) {
		for (int idy = 0; idy < window_height; ++idy) {
			float x2 = cos(angle) * (idx - x_center) - sin(angle) * (idy - y_center) + x_center;
			float y2 = sin(angle) * (idx - x_center) + cos(angle) * (idy - y_center) + y_center;

			framebuffer.setPixel(idx, idy, bola.getPixelSafe(x2, y2));
		}
	}
}

//funció que escala la imatge
void Application::scaleImage() {
	for (int idx = 0; idx < window_width; ++idx) {
		for (int idy = 0; idy < window_height; ++idy) {
			framebuffer.setPixel(idx, idy, image.getPixelSafe(idx * 0.1, idy * 0.1));
		}
	}
}

void Application::particleAnimation() {
	for (int idx = 0; idx < window_width; ++idx) {
		for (int idy = 0; idy < window_height; ++idy) {
			Color c = framebuffer.getPixelSafe(idx, idy);
			framebuffer.setPixelSafe(idx, idy, Color(c.r, c.g, c.b));
		}
	}
}

//funció per a tenir el nostre paint al framebuffer i poder pintar-lo
void Application::exercise6() {
	for (int idx = 0; idx < window_width; ++idx) {
		for (int idy = 0; idy < window_height; ++idy) {
			Color c = canvas.getPixelSafe(idx, idy);
			framebuffer.setPixel(idx, idy, c);
		}
	}
}

//En aquesta funció fem l'aplicació del paint
void Application::paint() {
	if (draw == true) {
		if (window_height - 30 <= mouse_position.y && mouse_position.y <= window_height - 10) {  //si tenim el ratolí dins de la finestra

			//segons on l'usuari cliqui, tindrem un color
			if (110 <= mouse_position.x && mouse_position.x <= 140) {
				color = Color::BLACK;
			}
			else if (160 <= mouse_position.x && mouse_position.x <= 190) {
				color = Color::RED;
			}
			else if (210 <= mouse_position.x && mouse_position.x <= 240) {
				color = Color::GREEN;
			}
			else if (260 <= mouse_position.x && mouse_position.x <= 290) {
				color = Color::BLUE;
			}
			else if (310 <= mouse_position.x && mouse_position.x <= 340) {
				color = Color::YELLOW;
			}
			else if (360 <= mouse_position.x && mouse_position.x <= 390) {
				color = Color::PURPLE;
			}
			else if (410 <= mouse_position.x && mouse_position.x <= 440) {
				color = Color::CYAN;
			}
			else if (460 <= mouse_position.x && mouse_position.x <= 490) {
				color = Color::WHITE;
			}
			else if (60 <= mouse_position.x && mouse_position.x <= 90) {
				canvas.saveTGA("../res/canvas.tga");
			}
		}

		framebuffer.drawLine(mouse_position.x, mouse_position.y, mouse_position.x + mouse_delta.x, mouse_position.y + mouse_delta.y, color);

		for (int idx = 0; idx < window_width; ++idx) {
			for (int idy = 0; idy < window_height; ++idy) {
				Color c = framebuffer.getPixelSafe(idx, idy);
				canvas.setPixel(idx, idy, c);
			}
		}
	}
}
