#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include <stdlib.h>
#include <glbinding/gl/gl.h>

#define _USE_MATH_DEFINES
#include <math.h>

// use gl definitions from glbinding 
using namespace gl;

//
#define NUM_STARS 1000
//#define NUM_POINTS_ORBIT 1200

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <iostream>

glm::vec4 origin;

float random(float min, float max) {
	return (max - min) * (float(rand() % 100) / 100) + min;
}

float makeOrbit(planet p) {
	return p.distanceToOrigin;
}

ApplicationSolar::ApplicationSolar(std::string const& resource_path):Application{resource_path}, planet_object{}, star_object {}
{
	//stars
	for (int i = 0; i < NUM_STARS; i++) {
		//Position				  min & max values of random
		star_buffer.push_back(random(-50, 50)); // x
		star_buffer.push_back(random(-50, 50)); // y
		star_buffer.push_back(random(-50, 50)); // z
												//Color
		star_buffer.push_back(random(0, 1)); // r
		star_buffer.push_back(random(0, 1)); // g
		star_buffer.push_back(random(0, 1)); // b
	}

	//orbits
	float increment = 2.0f *M_PI / 100;
	//orbits for planets
	for (int i = 0; i < sizeof(planets) / sizeof(planets[0]); i++) {
		//if (planets[i].isMoon == false) {
			float radius = makeOrbit(planets[i]);

			for (float rad = 0.0f; rad < 2.0f * M_PI; rad += increment) {
				orbit_buffer.push_back(radius * cos(rad)); // x
				orbit_buffer.push_back(0.0f); // y
				orbit_buffer.push_back(radius * sin(rad)); // z
														   //Color
				orbit_buffer.push_back(random(0, 1)); // x
				orbit_buffer.push_back(random(0, 1)); // y
				orbit_buffer.push_back(random(0, 1)); // z
			}
		//}
		
	}
	
  initializeGeometry();
  initializeShaderPrograms();

   
}

void ApplicationSolar::render() const {
  // bind shader to upload uniforms
  

  for (int i = 0; i < sizeof(planets) / sizeof(planets[0]); i++) {
	 
	  // iterating over planets, that are not moons
	  if (planets[i].isMoon == false) {
		  upload_planet_transforms(planets[i]);
	  }
  }

  // stars
  glUseProgram(m_shaders.at("star").handle);
  // bind the VAO to draw
  glBindVertexArray(star_object.vertex_AO);
  // draw bound vertex array using bound shader
  glDrawArrays(GL_POINTS, 0, NUM_STARS);


  upload_orbit_transforms();

 
}

void ApplicationSolar::upload_orbit_transforms() const{
	for (int i = 0; i < 12; i++) {
		glUseProgram(m_shaders.at("orbit").handle);
		// bind the VAO to draw
		glBindVertexArray(orbit_object.vertex_AO);

		if (planets[i].isMoon == false) {
			// draw bound vertex array using bound shader
			glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
				1, GL_FALSE, glm::value_ptr(glm::fmat4{}));

			glDrawArrays(GL_LINE_LOOP, i * 100, 100);
		}

		if (planets[i].isMoon == false && planets[i].hasMoonAtIndex > 0) {
			planet earth = planets[i];
			
			glm::fmat4 m_earth = glm::rotate(glm::fmat4{}, float(glfwGetTime() * earth.rotationSpeed), glm::fvec3{ 0.0f, 1.0f, 0.0f });

			 m_earth = glm::translate(m_earth, glm::fvec3{ 0.0f, 0.0f, earth.distanceToOrigin });
			
			glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
				1, GL_FALSE, glm::value_ptr(m_earth));

			glDrawArrays(GL_LINE_LOOP, earth.hasMoonAtIndex * 100, 100);
		}


		//else {
		//	int planetIndex = 2;
		//	for (int j = 0; j < 12; j++) {
		//		if (planets[j].hasMoonAtIndex == i) {
		//			planetIndex == j;
		//			std::cout << planets[planetIndex].name << std::endl;
		//		}
		//	}


		//	//glDrawArrays(GL_LINE_LOOP, i * 100, 100);
		//}

	}
		


	

	
}

// Assignment 1
void ApplicationSolar::upload_planet_transforms(planet newPlanet) const {
	glUseProgram(m_shaders.at("planet").handle);
	glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * newPlanet.rotationSpeed), glm::fvec3{ 0.0f, 1.0f, 0.0f });
	model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, newPlanet.distanceToOrigin });

	// code for the moon
	if (newPlanet.hasMoonAtIndex > 0) {
		int idx = newPlanet.hasMoonAtIndex;
		planet moon = planets[idx];

		glm::fmat4 model_matrix2 = glm::rotate(model_matrix, float(glfwGetTime() * moon.rotationSpeed), glm::fvec3{ 0.0f, 1.0f, 0.0f });

		model_matrix2 = glm::translate(model_matrix2, glm::fvec3{ 0.0f, 0.0f, moon.distanceToOrigin });
		model_matrix2 = glm::scale(model_matrix2, glm::fvec3{ moon.size, moon.size, moon.size });

		glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
			1, GL_FALSE, glm::value_ptr(model_matrix2));

		// extra matrix for normal transformation to keep them orthogonal to surface
		glm::fmat4 normal_matrix2 = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix2);
		glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
			1, GL_FALSE, glm::value_ptr(normal_matrix2));

		glm::vec3 planetColor(moon.colorR, moon.colorG, moon.colorB);
		glUniform3fv(m_shaders.at("planet").u_locs.at("DiffuseColor"), 1, glm::value_ptr(planetColor));

		// bind the VAO to draw
		glBindVertexArray(planet_object.vertex_AO);

		// draw bound vertex array using bound shader
		glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
	}


	model_matrix = glm::scale(model_matrix, glm::fvec3{ newPlanet.size, newPlanet.size, newPlanet.size });

	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));

	// extra matrix for normal transformation to keep them orthogonal to surface
	glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
	glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
		1, GL_FALSE, glm::value_ptr(normal_matrix));

	//assignment 3
	glm::vec3 planetColor(newPlanet.colorR, newPlanet.colorG, newPlanet.colorB);
	glUniform3fv(m_shaders.at("planet").u_locs.at("DiffuseColor"), 1, glm::value_ptr(planetColor));

	
	glm::fmat4 view_matrix = glm::inverse(m_view_transform);
	std::string str = newPlanet.name;

	if (!str.compare("sun")) {// is it si true ---> returns 0
		origin = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
	}
	else {
		origin = glm::vec4{ 0.0, 0.0, 0.0, 1.0 };
	}
	glm::vec4 sunPos4 = view_matrix*origin;
	glm::vec3 sunPos3(sunPos4);
	glUniform3fv(m_shaders.at("planet").u_locs.at("SunPosition"), 1, glm::value_ptr(sunPos3));
	


	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);

	// draw bound vertex array using bound shader
	glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}

void ApplicationSolar::updateView() {

	m_view_transform = glm::fmat4{};

	m_view_transform = glm::rotate(m_view_transform, -mouseX, glm::fvec3{ 0, 1.0f, 0 });
	m_view_transform = glm::rotate(m_view_transform, -mouseY, glm::fvec3{ 1.0f, 0, 0 });
	m_view_transform = glm::translate(m_view_transform, glm::fvec3{ slide, 0, zoom });
	m_view_transform = glm::scale(m_view_transform, glm::fvec3{ 50, 50, 50 });
	

	glUseProgram(m_shaders.at("planet").handle);
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
 
  //ass 3
  glm::vec4 sunPos4 = view_matrix*origin;
  glm::vec3 sunPos3(sunPos4);
  glUniform3fv(m_shaders.at("planet").u_locs.at("SunPosition"), 1, glm::value_ptr(sunPos3));


  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
	  1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("star").handle);
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
	  1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("orbit").handle);
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
	  1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("planet").handle);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
						1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("star").handle);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
	  1, GL_FALSE, glm::value_ptr(m_view_projection));
  glUseProgram(m_shaders.at("orbit").handle);

}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();
  
  // bind new shader
 // glUseProgram(m_shaders.at("planet").handle);
  
  updateView();
  //updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
	float speed = .2f;
	
	if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
		zoom -= speed;
		updateView();
	}
	else if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
		zoom += speed;
		updateView();
	}
	else if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
		slide -= speed;
		updateView();
	}
	else if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
		slide += speed;
		updateView();
	}
	else if (key == GLFW_KEY_1 && action != GLFW_RELEASE) {
		glUseProgram(m_shaders.at("planet").handle);
		glUniform1f(m_shaders.at("planet").u_locs.at("ShaderMode"),1);
		updateView();
	}
	else if (key == GLFW_KEY_2 && action != GLFW_RELEASE) {
		glUseProgram(m_shaders.at("planet").handle);
		glUniform1f(m_shaders.at("planet").u_locs.at("ShaderMode"), 2);
		updateView();
	}

 
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
	if (mouseActive) { // in case if we have a mouse down event
		float speed = .01f;
		mouseX += pos_x * speed;
		mouseY += pos_y * speed;
		updateView();
	}

	
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert", m_resource_path + "shaders/simple.frag"});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["DiffuseColor"] = -1;
  m_shaders.at("planet").u_locs["SunPosition"] = -1;
  m_shaders.at("planet").u_locs["ShaderMode"] = -1;

  // store shader program objects in container
  m_shaders.emplace("star",shader_program{ m_resource_path + "shaders/star.vert", m_resource_path + "shaders/star.frag" });
  // request uniform locations for shader program
  m_shaders.at("star").u_locs["ViewMatrix"] = -1;
  m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

  // store shader program objects in container
  m_shaders.emplace("orbit", shader_program{ m_resource_path + "shaders/orbit.vert", m_resource_path + "shaders/orbit.frag" });
  // request uniform locations for shader program
  m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ModelMatrix"] = -1;

}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());


  //======================STARS==================
  model star_model = {star_buffer,model::POSITION | model::NORMAL};

  // generate vertex array object
  glGenVertexArrays(1, &star_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(star_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &star_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * star_model.data.size(), star_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::NORMAL]);

  // generate generic buffer
  glGenBuffers(1, &star_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * star_model.indices.size(), star_model.indices.data(), GL_STATIC_DRAW);



  //===============ORBIT=================================
  
  
  model orbit_model = { orbit_buffer,model::POSITION | model::NORMAL };

  // generate vertex array object
  glGenVertexArrays(1, &orbit_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(orbit_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &orbit_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * orbit_model.data.size(), orbit_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, orbit_model.vertex_bytes, orbit_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, orbit_model.vertex_bytes, orbit_model.offsets[model::NORMAL]);

  // generate generic buffer
  glGenBuffers(1, &orbit_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, orbit_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * orbit_model.indices.size(), orbit_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  orbit_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  orbit_object.num_elements = GLsizei(orbit_model.indices.size());

}



ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteBuffers(1, &star_object.element_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);

  glDeleteBuffers(1, &orbit_object.vertex_BO);
  glDeleteBuffers(1, &orbit_object.element_BO);
  glDeleteVertexArrays(1, &orbit_object.vertex_AO);
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}