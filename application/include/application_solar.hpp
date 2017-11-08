#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

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
  void upload_planet_transforms(planet newPlanet) const;
  void upload_orbit_transforms() const;

  // initalization mouse
private:
	//mouse control
	bool mouseActive = true;
	float mouseX = 0;
	float mouseY = 0;
	float zoom = 30; // init camera distance
	float slide = 0;

	//buffers
	std::vector<float> star_buffer;
	std::vector<float> orbit_buffer;

	// cpu representation of model
	model_object planet_object;
	model_object star_object;
	model_object orbit_object;
 
  

  //						name, size, rotation speed, distance to origin, has MoonAt index, isMoon
  planet planets[12] = { { "sun",	3.0f, 5.0f, 0.0f,	-1,	false },
						{ "mercury",0.5f, 1.8f, 5.0f,	-1, false },
						{ "venus",	1.3f, 1.0f, 10.0f,  -1, false },
						{ "earth",	2.2f, 1.5f, 15.0f,	 4, false },
						{ "moon",	0.5f, 6.1f, 3.7f,	-1, true  },//
					    { "mars",	2.0f, 2.1f, 20.7f,	-1, false }, 
					    { "jupiter",1.0f, 2.5f, 25.7f,	 7, false }, 
						{ "europa", 0.5f, 6.1f, 1.7f,	-1, true  },//
						{ "saturn", 2.0f, 3.6f, 30.7f,	-1, false },
						{ "uranus", 1.5f, 2.9f, 35.7f,	-1, false },
						{ "neptune", 1.7f, 1.1f, 40.7f,	-1, false },
						{ "pluto",	 1.5f, 2.2f, 45.7f,	-1, false } };


  

};

#endif