#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

#define NUM_PLANETS 13

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();


  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);

  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();
  void upload_planet_transforms(int i) const;
  void upload_orbit_transforms() const;
  void loadTextures();
  
  // initalization mouse
private:
	//mouse control
	bool mouseActive = true;
	float mouseX = 0;
	float mouseY = 0;
	float zoom = 30; // init camera distance
	float slide = 0;
	GLuint texture_object[NUM_PLANETS+1];

	

	//buffers
	std::vector<float> star_buffer;
	std::vector<float> orbit_buffer;
	

	// cpu representation of model
	model_object planet_object;
	model_object star_object;
	model_object orbit_object;
 
  

  //						name, size, rotation speed, distance to origin, has MoonAt index, isMoon, colorRGB
  planet planets[NUM_PLANETS] = {
						{ "sun",	3.0f, 1.0f, 0.0f,	-1,	false, 0.9f, 0.7f, 0.2f },
						{ "mercury",0.5f, 0.8f, 5.0f,	-1, false, 1.0f, 1.0f, 0.0f },
						{ "venus",	1.3f, 0.2f, 10.0f,  -1, false, 1.0f, 0.0f, 1.0f },
						{ "earth",	2.2f, 0.1f, 15.0f,	 4, false, 0.0f, 1.0f, 1.0f },
						{ "moon",	0.5f, 2.1f, 3.7f,	-1, true , 0.0f, 1.0f, 0.0f },//
					    { "mars",	2.0f, 1.1f, 20.7f,	-1, false, 1.0f, 1.0f, 1.0f },
					    { "jupiter",1.0f, 1.2f, 25.7f,	 7, false, 0.3f, 0.4f, 1.0f },
						{ "europa", 0.5f, 1.5f, 1.7f,	-1, true , 0.2f, 0.2f, 0.2f },//
						{ "saturn", 2.0f, 1.0f, 30.7f,	-1, false, 1.0f, 0.4f, 0.9f },
						{ "uranus", 1.5f, 0.9f, 35.7f,	-1, false, 0.1f, 0.3f, 0.4f },
						{ "neptune", 1.7f, 0.1f, 40.7f,	-1, false, 0.4f, 0.2f, 0.9f },
						{ "pluto",	 1.5f, 1.2f, 45.7f,	-1, false, 0.0f, 1.0f, 0.4f },
						{ "skybox", 50.0f, 0.0f, 0.0f,	-1, false, 0.0f, 0.0f, 1.0f }};


  

};

#endif