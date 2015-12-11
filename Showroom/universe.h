#pragma once
#define _USE_MATH_DEFINES // for C++
#include <cmath>

#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>

#include <tiny_obj_loader.h>

#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <SFML\Graphics\Image.hpp>

class universe
{
public:
	universe();
	~universe();

	bool addObject(char* dir, char* file);
	void run();
	void resize();
	void light();
	void draw(tinyobj::shape_t& shape, tinyobj::material_t& mat);
	void display(std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials, sf::Vector3f& pos, float yrot);
	void init();
	float distance(sf::Vector3f& a, sf::Vector3f& b);
private:
	std::vector<std::vector<tinyobj::shape_t>> objects;
	std::vector<std::vector<tinyobj::material_t>> objects_mat;

	std::vector<sf::Vector3f> abarths;

	/*float	cam_z = 0.0f,
		cam_x = 0.0f,
		cam_y = -10.0f,
		cam_h = 90.0f,
		cam_v = 0.0f;*/
	sf::Window m_window;

	sf::Clock clock;
	float dt = 0.016f;

	sf::Vector3f m_camPos;
	sf::Vector2f m_camRot;

	sf::Vector3f m_light;

	//bool toggleColor = false;

	GLfloat redDiffuseMaterial[3] = { 1.0, 0.0, 0.0 }; //set the material to red
	GLfloat whiteSpecularMaterial[3] = { 1.0, 1.0, 1.0 }; //set the material to white
	GLfloat greenEmissiveMaterial[3] = { 0.0, 1.0, 0.0 }; //set the material to green
	GLfloat whiteSpecularLight[3] = { 1.0, 1.0, 1.0 }; //set the light specular to white
	GLfloat blackAmbientLight[3] = { 0.5, 0.5, 0.5 }; //set the light ambient to black
	GLfloat whiteDiffuseLight[3] = { 1.0, 1.0, 1.0 }; //set the diffuse light to white
	GLfloat blankMaterial[3] = { 0.0, 0.0, 0.0 }; //set the diffuselight to white
	GLfloat mShininess[1] = { 64 }; //set the shininess of the material

	enum Quality { Q_LOW = 2, Q_MID = 1, Q_HIGH = 0, Q_ALL = -1 };
};

