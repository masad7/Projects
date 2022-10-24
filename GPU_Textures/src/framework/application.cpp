#include "application.h"
#include "utils.h"
#include "includes.h"
#include "utils.h"

#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "light.h"

Camera* camera = NULL;
Mesh* mesh = NULL;
Shader* shader = NULL;

//might be useful...
Material* material = NULL;

Light* light = NULL;
Light* light_2 = NULL;

Shader* phong_shader = NULL;
Shader* gouraud_shader = NULL;

std::vector<Matrix44> models;
std::vector<Light*> lights;


Vector3 ambient_light(0.1,0.2,0.3); //here we can store the global ambient light of the scene

float angle = 0;

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
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
	if( !mesh->loadOBJ( "../res/meshes/lee.obj" ) )
		std::cout << "FILE Lee.obj NOT FOUND " << std::endl;

	//we load one or several shaders...
	shader = Shader::Get( "../res/shaders/simple.vs", "../res/shaders/simple.fs" );

	//load your Gouraud and Phong shaders here and stored them in some global variables
	//...
	gouraud_shader = Shader::Get("../res/shaders/gouraud.vs", "../res/shaders/gouraud.fs");
	phong_shader = Shader::Get("../res/shaders/phong.vs", "../res/shaders/phong.fs");



	//CODE HERE:
	//create a light (or several) and and some materials
	material = new Material();
	light = new Light();
	
	//llum extra
	light_2 = new Light();
	light_2->position = Vector3(-50.0, 50.0, 0.0);
	light_2->diffuse_color.set(0.3f, 0.9f, 1.0f);
	light_2->specular_color.set(0.5f, 0.6f, 0.1f);

	//més llums
	lights.resize(4);

	for (int i = 0; i < lights.size(); i++) { //creem instàncies de llum
		lights[i] = new Light();
	}
	lights[3] = light_2;

	lights[0]->diffuse_color.set(0.8, 0, 0);
	lights[0]->specular_color.set(0.8, 0.8, 0);
	lights[0]->position.set(-50, 50, -50);

	lights[1]->diffuse_color.set(0.7, 0, 0.7);
	lights[1]->specular_color.set(0.7, 0, 0.7);
	lights[1]->position.set(-50, -50, 0);

	lights[2]->diffuse_color.set(0, 0.8, 0.8);
	lights[2]->specular_color.set(0, 0.6, 0.8);
	lights[2]->position.set(50, 50, 0);


	//número de figures
	models.resize(5);
	for (int idx = 0; idx < models.size(); ++idx) {
		models[idx] = *new Matrix44();
	}

	llums = 3;
}

//render one frame
void Application::render(void)
{
	//update the aspect of the camera acording to the window size
	camera->aspect = window_width / window_height;
	camera->updateProjectionMatrix();
	//Get the viewprojection matrix from our camera
	Matrix44 viewprojection = camera->getViewProjectionMatrix();

	//set the clear color of the colorbuffer as the ambient light so it matches
	glClearColor(ambient_light.x, ambient_light.y, ambient_light.z, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear framebuffer and depth buffer 
	glEnable( GL_DEPTH_TEST ); //enable depth testing for occlusions
	glDepthFunc(GL_LEQUAL); //Z will pass if the Z is LESS or EQUAL to the Z of the pixel

	//choose a shader and enable it
	//shader->enable();

	Matrix44 model_matrix;
	model_matrix.setIdentity();
	model_matrix.translate(0,0,0); //example of translation
	model_matrix.rotate(angle, Vector3(0, 1, 0));
	
	//shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
	//shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

	//CODE HERE: pass all the info needed by the shader to do the computations
	if (mode == 1) { //gouraud
		//choose a shader and enable it
		gouraud_shader->enable();

		gouraud_shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
		gouraud_shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

		//send the material and light uniforms to the shader
		gouraud_shader->setVector3("ka", material->ambient);
		gouraud_shader->setVector3("kd", material->diffuse);
		gouraud_shader->setVector3("ks", material->specular);
		
		gouraud_shader->setVector3("Ia", ambient_light);
		gouraud_shader->setVector3("Id", light->diffuse_color);
		gouraud_shader->setVector3("Is", light->specular_color);

		gouraud_shader->setFloat("alpha", material->shininess);

		gouraud_shader->setVector3("eye_position", camera->eye);
		gouraud_shader->setVector3("light_position", light->position);

		//do the draw call into the GPU
		mesh->render(GL_TRIANGLES);

		//disable shader when we do not need it any more
		gouraud_shader->disable();
	}
	
	if (mode == 2) { //phong
		phong_shader->enable();

		phong_shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
		phong_shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

		//send the material and light uniforms to the shader
		phong_shader->setVector3("ka", material->ambient);
		phong_shader->setVector3("kd", material->diffuse);
		phong_shader->setVector3("ks", material->specular);
		
		phong_shader->setVector3("Ia", ambient_light);
		phong_shader->setVector3("Id", light->diffuse_color);
		phong_shader->setVector3("Is", light->specular_color);

		phong_shader->setFloat("alpha", material->shininess);

		phong_shader->setVector3("eye_position", camera->eye);
		phong_shader->setVector3("light_position", light->position);

		//do the draw call into the GPU
		mesh->render(GL_TRIANGLES);

		//disable shader when we do not need it any more
		phong_shader->disable();
	
	}

	if (mode == 3) { //phong amb una llum
		phong_shader->enable();
		glDepthFunc(GL_LEQUAL);

		phong_shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
		phong_shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

		//send the material and light uniforms to the shader
		phong_shader->setVector3("ka", material->ambient);
		phong_shader->setVector3("kd", material->diffuse);
		phong_shader->setVector3("ks", material->specular);

		phong_shader->setVector3("Ia", ambient_light);
		phong_shader->setVector3("Id", light->diffuse_color);
		phong_shader->setVector3("Is", light->specular_color);

		phong_shader->setFloat("alpha", material->shininess);

		phong_shader->setVector3("eye_position", camera->eye);
		phong_shader->setVector3("light_position", light->position);


		glDisable(GL_BLEND);

		//do the draw call into the GPU
		mesh->render(GL_TRIANGLES);

		phong_shader->setVector3("Ia", Vector3(0.0, 0.0, 0.0));
		phong_shader->setVector3("Id", light_2->diffuse_color);
		phong_shader->setVector3("Is", light_2->specular_color);

		phong_shader->setVector3("light_position", light_2->position);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		mesh->render(GL_TRIANGLES);
		glDisable(GL_BLEND);
		phong_shader->disable();
	}

	if (mode == 4) { //render the same mesh several times

		models[0].setIdentity();
		models[0].translate(0, 0, 0); //example of translation
		models[0].rotateLocal(angle, Vector3(0, 1, 0));

		phong_shader->enable();

		phong_shader->setMatrix44("model", models[0]); //upload the transform matrix to the shader
		phong_shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

		//send the material and light uniforms to the shader
		phong_shader->setVector3("ka", material->ambient);
		phong_shader->setVector3("ks", material->specular);
		phong_shader->setVector3("kd", material->diffuse);

		phong_shader->setVector3("Ia", ambient_light);
		phong_shader->setVector3("Is", light->specular_color);
		phong_shader->setVector3("Id", light->diffuse_color);

		phong_shader->setFloat("alpha", material->shininess);

		phong_shader->setVector3("camara_position", camera->eye); //posicio de la camara
		phong_shader->setVector3("light_position", light->position); //posicio del punt llum

		mesh->render(GL_TRIANGLES);

		for (int i = 1; i < models.size(); i++) {

			models[i].setIdentity();
			models[i].translate(10*i, 0, -5); //example of translation
			models[i].rotateLocal(angle, Vector3(1, 0, 0));

			phong_shader->setMatrix44("model", models[i]); //upload the transform matrix to the shader
			mesh->render(GL_TRIANGLES);

		}
	}
	

	//do the draw call into the GPU
	//mesh->render(GL_TRIANGLES);
	

	//disable shader when we do not need it any more
	//shader->disable();

	if (mode == 5) {
		phong_shader->enable();
		glDepthFunc(GL_LEQUAL);

		phong_shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
		phong_shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

		//send the material and light uniforms to the shader
		phong_shader->setVector3("ka", material->ambient);
		phong_shader->setVector3("kd", material->diffuse);
		phong_shader->setVector3("ks", material->specular);

		phong_shader->setVector3("Ia", ambient_light);
		

		phong_shader->setFloat("alpha", material->shininess);

		phong_shader->setVector3("eye_position", camera->eye);

		mesh->render(GL_TRIANGLES);

		for (int idx = 0; idx < lights.size() - 1; ++idx) {
			if (idx == 0) {
				glDisable(GL_BLEND);
				phong_shader->setVector3("Id", lights[idx]->diffuse_color);
				phong_shader->setVector3("Is", lights[idx]->specular_color);

				phong_shader->setVector3("light_position", lights[idx]->position);

				mesh->render(GL_TRIANGLES);
			}
			else {
				glEnable(GL_BLEND);

				phong_shader->setVector3("Id", lights[idx]->diffuse_color);
				phong_shader->setVector3("Is", lights[idx]->specular_color);

				phong_shader->setVector3("light_position", lights[idx]->position);

				glBlendFunc(GL_ONE, GL_ONE);
				mesh->render(GL_TRIANGLES);

			}
		}
		phong_shader->disable();

	}


	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}


//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
		angle += seconds_elapsed;

	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_LEFT])
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_DOWN])
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;

	//tecles tasques
	if (keystate[SDL_SCANCODE_1]) {
		mode = 1;
	}
	if (keystate[SDL_SCANCODE_2]) {
		mode = 2;
	}
	if (keystate[SDL_SCANCODE_3]) {
		mode = 3;
	}
	if (keystate[SDL_SCANCODE_4]) {
		mode = 4;
	}
	if (keystate[SDL_SCANCODE_5]) {
		mode = 5;
	}

	//rotació en òrbita
	if (keystate[SDL_SCANCODE_A]) {
		Matrix44 R;
		R.setRotation(seconds_elapsed, Vector3(0, 1, 0));

		Vector3 front = R * (camera->eye - camera->center);
		camera->eye = camera->center + front;
	}

	if (keystate[SDL_SCANCODE_W]) {
		Matrix44 R;
		R.setRotation(seconds_elapsed, Vector3(1,0,0));

		Vector3 front = R * (camera->eye - camera->center);
		camera->eye = camera->center + front;
	}

	//canvi de llums
	if (keystate[SDL_SCANCODE_L]) {
		llums = 0;
	}
	if (keystate[SDL_SCANCODE_K]) {
		llums = 1;
	}	
	if (keystate[SDL_SCANCODE_J]) {
		llums = 2;
	}
	if (keystate[SDL_SCANCODE_H]) {
		llums = 3;
	}

}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDLK_r: 
			Shader::ReloadAll();
			break; //ESC key, kill the app
	}
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		if (llums == 0) {
			lights[0]->position.x = mouse_position.x;
			lights[0]->position.y = mouse_position.y;
		}
		else if (llums == 1) {
			lights[1]->position.x = mouse_position.x;
			lights[1]->position.y = mouse_position.y;
		}
		else if (llums == 2) {
			lights[2]->position.x = mouse_position.x;
			lights[2]->position.y = mouse_position.y;
		}
		else if (llums == 3) {
			light_2->position.x = mouse_position.x;
			light_2->position.y = mouse_position.y;

			lights[3]->position.x = mouse_position.x;
			lights[3]->position.y = mouse_position.y;
		}
	}

}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
