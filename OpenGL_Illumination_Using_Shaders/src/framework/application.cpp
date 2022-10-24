#include "application.h"
#include "utils.h"
#include "image.h"
#include "mesh.h"


Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);

	zbuffer.resize(w, h);
	framebuffer.resize(w, h);
}

//Here we have already GL working, so we can create meshes and textures
//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,10,20),Vector3(0,10,0),Vector3(0,1,0)); //define eye,center,up
	camera->perspective(60, window_width / (float)window_height, 0.1, 10000); //define fov,aspect,near,far

	//load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ("lee.obj") )
		std::cout << "FILE Lee.obj NOT FOUND" << std::endl;

	//load the texture
	texture = new Image();
	texture->loadTGA("color.tga");


	framebuffer_copy = framebuffer;
}

//this function fills the triangle by computing the bounding box of the triangle in screen space and using the barycentric interpolation
//to check which pixels are inside the triangle. It is slow for big triangles, but faster for small triangles 
void Application::fillTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector2& uv0, const Vector2& uv1, const Vector2& uv2)
{

}

//render one frame
void Application::render(Image& framebuffer)
{
	framebuffer.fill(Color(40, 45, 60)); //clear
	zbuffer.fill(10000);

	//for every point of the mesh (to draw triangles take three points each time and connect the points between them (1,2,3,   4,5,6,   ...)
	Vector2 uv1, uv2, uv3;

	for (int i = 0; i < mesh->vertices.size(); ++i)
	{
		int x0, y0, x1, y1, x2, y2;
		Vector3 vertex1 = mesh->vertices[i]; //extract vertex from mesh
		Vector3 firstvertex = transform3Dto2D(vertex1);
		x0 = firstvertex.x;
		y0 = firstvertex.y;
		uv1 = mesh->uvs[i];

		i++;
		Vector3 vertex2 = mesh->vertices[i]; //extract vertex from mesh
		Vector3 secondvertex = transform3Dto2D(vertex2);
		x1 = secondvertex.x;
		y1 = secondvertex.y;
		uv2 = mesh->uvs[i];


		i++;
		Vector3 vertex3 = mesh->vertices[i]; //extract vertex from mesh
		Vector3 thirdvertex = transform3Dto2D(vertex3);
		x2 = thirdvertex.x;
		y2 = thirdvertex.y;
		uv3 = mesh->uvs[i];

		uv1.x = texture->width * (uv1.x);
		uv1.y = texture->height * (uv1.y);
		
		uv2.x = texture->width * (uv2.x);
		uv2.y = texture->height * (uv2.y);

		uv3.x = texture->width * (uv3.x);
		uv3.y = texture->height * (uv3.y);


		
		//ACTIVITAT 7: INTERACCIÓ
		if (status == 1) {
			framebuffer.drawTriangle(firstvertex.x, firstvertex.y, secondvertex.x, secondvertex.y, thirdvertex.x, thirdvertex.y, Color::WHITE, false);

		}
		if (status == 2) {
			framebuffer.drawTriangleInterpolated(x0, y0, x1, y1, x2, y2, Color::RED, Color::GREEN, Color::BLUE);

		}
		if (status == 3) {
			framebuffer.Zbuffer(firstvertex, secondvertex, thirdvertex, &zbuffer);
		}
		if(status == 4){
			framebuffer.texturing(firstvertex, secondvertex, thirdvertex, uv1, uv2, uv3, texture, &zbuffer);
		}
		if (status == 5) {
			framebuffer.fill(Color::BLACK);
		}
		
	}

}

Vector3 Application::transform3Dto2D(Vector3 vertex) {

	Vector3 finalvertex = camera->projectVector(vertex);

	if (finalvertex.x < 1 && finalvertex.x>-1 && finalvertex.y < 1 && finalvertex.y>-1) {

		finalvertex.x = (window_width / 2) * finalvertex.x + (window_width / 2); 
		finalvertex.y =(window_height/2)*finalvertex.y + (window_height / 2);  

	}
	return finalvertex;
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (keystate[SDL_SCANCODE_W])
	{
		camera->center.y -= 5 * seconds_elapsed;
	}

	if (keystate[SDL_SCANCODE_S]) {
		camera->center.y += 15 * seconds_elapsed;

	}

	if (keystate[SDL_SCANCODE_A]) {
		camera->center.x += 15 * seconds_elapsed;
	}

	if (keystate[SDL_SCANCODE_D]) {
		camera->center.x -= 15 * seconds_elapsed;

	}

	if(keystate[SDL_SCANCODE_F]) {
		camera->fov += 15 * seconds_elapsed;
	}


	if (keystate[SDL_SCANCODE_G]) {
		camera->fov -= 15 * seconds_elapsed;
	}

	//example to move eye
	if (keystate[SDL_SCANCODE_LEFT]) {

		camera->eye.x -= 15 * seconds_elapsed;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		camera->eye.x += 15 * seconds_elapsed;
	}

	camera->updateViewMatrix();
	camera->updateProjectionMatrix();
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
		case SDL_SCANCODE_ESCAPE: exit(0); break; //ESC key, kill the app
		
		case SDL_SCANCODE_1:
			Application::framebuffer = Application::framebuffer_copy;
			status = 1;
			break;
		
		case SDL_SCANCODE_2:
			Application::framebuffer = Application::framebuffer_copy;
			status = 2;
			break;

		case SDL_SCANCODE_3:
			Application::framebuffer = Application::framebuffer_copy;
			status = 3;
			break;
		
		case SDL_SCANCODE_4:
			Application::framebuffer = Application::framebuffer_copy;
			status = 4;
			break;
	}
}

//keyboard released event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	switch (event.keysym.scancode)
	{
	}
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		x = mouse_position.x;
		y = mouse_position.y;

	}
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{
		camera->eye.x = camera->eye.x + (x - mouse_position.x) / 10;
		camera->eye.y = camera->eye.y + (y - mouse_position.y) / 10;

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
