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
	canvas.loadTGA("../res/canvas.tga");
	canvas.scale(window_width, window_height);
}

//render one frame
void Application::render(Image& framebuffer)
{
	//clear framebuffer if we want to start from scratch
	//framebuffer.fill(Color::BLACK);

	//CRIDA DE TOTES LES FUNCIONS
	paint();
	ex();

	if (exercise == 6) {
		//clear framebuffer if we want to start from scratch
		framebuffer.fill(Color::BLACK);

		Application::framebuffer.drawTriangleInterpolated(500, 200, 625, 50, 750, 200, Color::YELLOW, Color::BLUE, Color::GREEN);
		Application::framebuffer.drawTriangleInterpolated(400, 300, 100, 400, 500, 500, Color::BLUE, Color::YELLOW, Color::WHITE);
		Application::framebuffer.drawTriangleInterpolated(500, 400, 600, 500, 700, 350, Color::BLACK, Color::BLUE, Color::GREEN);
		Application::framebuffer.drawTriangleInterpolated(100, 50, 300, 100, 400, 20, Color::WHITE, Color::RED, Color::PURPLE);
		Application::framebuffer.drawTriangleInterpolated(100, 50, 500, 300, 100, 200, Color::CYAN, Color::PURPLE, Color::BLUE);
		Application::framebuffer.drawTriangleInterpolated(30, 400, 30, 550, 100, 488, Color::GREEN, Color::BLUE, Color::RED);

	}
	else if (exercise == 7) {
		//clear framebuffer if we want to start from scratch
		framebuffer.fill(Color::BLACK);
	}

}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	
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
		Application::exercise = 1;  //tecla 1 dibuixa líies DDA
		break;
	case SDL_SCANCODE_2:
		Application::exercise = 2;  //tecla 2 dibuixa línies Bresenham
		break;
	case SDL_SCANCODE_3:
		Application::exercise = 3;  //tecla 3 dibuixa contorn cercle
		break;
	case SDL_SCANCODE_4:
		Application::exercise = 4;  //tecla 4 dibuixa cercle emplenat de color
		break;
	case SDL_SCANCODE_5:
		Application::exercise = 5;  //tecla 5 dibuixa triangles
		break;
	case SDL_SCANCODE_6:
		Application::exercise = 6;  //tecla 6 dibuixa triangles interpolats
		break;
	case SDL_SCANCODE_7:
		Application::exercise = 7;  //tecla 7 clear the framebuffer
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
		draw = true; //activem paint
		
		//variables que ens guarden la posició del cursor
		Application::x = mouse_position.x;
		Application::y = mouse_position.y;
		
		Application::count = 0; 
		if (count == 0) {
			Application::tx0 = mouse_position.x;
			Application::ty0 = mouse_position.y;
			Application::count++;
		}
		else if (count == 1) {
			Application::tx1 = mouse_position.x;
			Application::ty1 = mouse_position.y;
			Application::count++;
		}
		else if (count == 2) {
			Application::tx2 = mouse_position.x;
			Application::ty2 = mouse_position.y;
			Application::count++;
		}

		
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

//funció per a tenir el nostre paint al framebuffer i poder pintar-lo
void Application::ex() {
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

			//segons on l'usuari cliqui, agafem un color
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
			else if (5 <= mouse_position.x && mouse_position.x <= 30) {
				for (int x = 0; x < 800; x++) {
					for (int y = 0; y < 600 ; y++) {
						canvas.setPixel(x, y, Color::WHITE);
					}
				}
			}
		}
		
		
		//segons quina tecla es cliqui, ens dibuixarà una figura

		if (exercise == 1) { //tecla 1, dibuixa linia amb algoritme DDA
			framebuffer.drawLineDDA(x, y, mouse_position.x, mouse_position.y, color);
		}
		else if (exercise == 2) { //tecla 2, dibuixa linia amb algoritme Bresenham
			framebuffer.drawLineBresenham(x, y, mouse_position.x, mouse_position.y, color);
		}
		else if (exercise == 3) { //tecla 3, dibuixa circumferència
			int radi = 10 + rand() % 150;
			framebuffer.drawCircle(x, y, radi, color, false);
		}
		else if (exercise == 4) { //tecla 4, dibuixa cercle
			int radi = 10 + rand() % 150;
			framebuffer.drawCircle(x, y, radi, color, true);
		}
		else if (exercise == 5) { //tecla 5, dibuixa triangles
			Application::framebuffer.drawTriangle(500, 200, 625, 50, 750, 200, color, true);
			Application::framebuffer.drawTriangle(400, 300, 100, 400, 500, 500, color, true);
			Application::framebuffer.drawTriangle(100, 100, 200, 200, 300, 250, color, true);
			Application::framebuffer.drawTriangle(50, 100, 100, 300, 500, 550, color, false);
			Application::framebuffer.drawTriangle(500, 400, 600, 500, 700, 450, color, false);
			Application::framebuffer.drawTriangle(500, 400, 500, 500, 600, 400, color, true);
		}

		for (int idx = 0; idx < window_width; ++idx) {
			for (int idy = 0; idy < window_height; ++idy) {
				Color c = framebuffer.getPixelSafe(idx, idy);
				canvas.setPixel(idx, idy, c);
			}
		}

	}
}
