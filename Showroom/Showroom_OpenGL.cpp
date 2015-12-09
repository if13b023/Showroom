// Showroom.cpp : Defines the entry point for the console application.
//
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

float	cam_z = 0.0f,
		cam_x = 0.0f,
		cam_y = -10.0f,
		cam_h = 0.0f,
		cam_v = 0.0f;

//bool toggleColor = false;

GLfloat redDiffuseMaterial[] = { 1.0, 0.0, 0.0 }; //set the material to red
GLfloat whiteSpecularMaterial[] = { 1.0, 1.0, 1.0 }; //set the material to white
GLfloat greenEmissiveMaterial[] = { 0.0, 1.0, 0.0 }; //set the material to green
GLfloat whiteSpecularLight[] = { 1.0, 1.0, 1.0 }; //set the light specular to white
GLfloat blackAmbientLight[] = { 0.5, 0.5, 0.5 }; //set the light ambient to black
GLfloat whiteDiffuseLight[] = { 1.0, 1.0, 1.0 }; //set the diffuse light to white
GLfloat blankMaterial[] = { 0.0, 0.0, 0.0 }; //set the diffuselight to white
GLfloat mShininess[] = { 64 }; //set the shininess of the material

void resize(sf::Window& window)
{
	int height = window.getSize().y;
	int width = window.getSize().x;
	// prevent division by zero
	if (window.getSize().y == 0) { height = 1; }

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
	glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
	glMatrixMode(GL_MODELVIEW);
}

void light(void)
{
	GLfloat lightPos[] = { 0.0f, 10.0f, 0.0f , 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
}

void display(std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t> materials, sf::Image& texfile)
{
	GLfloat material[] = { 1.0, 0.0, 1.0 };
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, material);

	//gluLookAt(sinf(rot) * 10, 0, cosf(rot) * 10, 0, 0, 0, 0, 1, 0);

	light();

	glPushMatrix();
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//use: glDrawElements
	for (int i = 0; i < shapes.size(); ++i)
	{
		float* diffuse = materials[shapes[i].mesh.material_ids[0]].diffuse;
		//float diffuse[] = { 0, 0, 1.0 };
		material[0] = diffuse[0];
		material[1] = diffuse[1];
		material[2] = diffuse[2];

		//glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
		//glMaterialfv(GL_FRONT, GL_SHININESS, mShininess);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, material);

		glTexCoordPointer(2, GL_FLOAT, 0, shapes[i].mesh.texcoords.data());
		glVertexPointer(3, GL_FLOAT, 0, shapes[i].mesh.positions.data());
		glNormalPointer(GL_FLOAT, 0, shapes[i].mesh.normals.data());

		glDrawElements(GL_TRIANGLES, shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, shapes[i].mesh.indices.data());
	}

	//glVertexPointer(3, GL_FLOAT, 0, shapes[0].mesh.positions.data());
	//glDrawElements(GL_TRIANGLES, shapes[0].mesh.indices.size(), GL_UNSIGNED_INT, shapes[0].mesh.indices.data());

	//use: glDrawArrays
	//glVertexPointer(3, GL_FLOAT, 0, shapes[0].mesh.positions.data());
	//glDrawArrays(GL_TRIANGLES, 0, shapes[0].mesh.positions.size());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 5);
	//glutSolidTeapot(2);
	glPopMatrix();

	//glutSwapBuffers();

	int err = GL_NO_ERROR;
	if ((err = glGetError()) != GL_NO_ERROR)
		std::cout << "Error: " << err << std::endl;
}

void init(sf::Window& window)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	//glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	//glEnable(GL_COLOR_MATERIAL);
	//glShadeModel(GL_FLAT);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	resize(window);
}

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
		std::cout << "Argument: " << argv[i] << std::endl;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::vector<std::string> inputfiles;
	std::string inputdir;

	if (argc >= 3)
	{
		inputdir = argv[1];
		for (int i = 2; i < argc; ++i)
			inputfiles.push_back(argv[i]);
	}
	else
	{
		std::string in;
		std::cin >> inputdir;
		std::cin >> in;
		inputfiles.push_back(in);
	}

	//std::vector<tinyobj::shape_t> shapes;
	//std::vector<tinyobj::material_t> materials;

	//Creating the window
	sf::Window window(sf::VideoMode(800, 600), "Showroom OpenGL", sf::Style::Default, sf::ContextSettings(32));
	//***

	std::string err;
	bool ret = false;

	for (int i = 0; i < inputfiles.size(); ++i)
		ret = tinyobj::LoadObj(shapes, materials, err, (inputdir + inputfiles.at(i)).c_str(), inputdir.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cout << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	//OpenGL stuff
	init(window);
	//***

	//pre main init
	sf::Clock clock;
	float dt = 0.016f;
	sf::Vector2f mouse_old(0, 0);
	//***

	//Texture
	sf::Image texfile;
	if (texfile.loadFromFile("../TestTex.png") == 0)
	{
		std::cout << "can not load texture...\n";
		return 666;
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texfile.getSize().x, texfile.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, texfile.getPixelsPtr());
	//***

	//main loop
	bool quit = false;
	while (!quit)
	{
		clock.restart();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				// end the program
				quit = true;
			}
			else if (event.type == sf::Event::Resized)
			{
				// adjust the viewport when the window is resized
				//glViewport(0, 0, event.size.width, event.size.height);
				resize(window);
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
					quit = true;
				else if(event.key.code == sf::Keyboard::Num1)
					std::cout << cam_h << std::endl;
			}
		}

		//Mouse Input
		float sens = 3.0f;
		//sf::Vector2f mouse_diff = mouse_old - sf::Vector2f(sf::Mouse::getPosition());
		sf::Vector2f mouse_diff = sf::Vector2f(sf::Mouse::getPosition());
		cam_h = (mouse_diff.x / sf::VideoMode::getDesktopMode().width) * 360.0f * sens;
		cam_v = (mouse_diff.y / sf::VideoMode::getDesktopMode().height) * 360.0f * sens;

		while (cam_h > 360.0) cam_h -= 360.0f;
		while (cam_h < 0.0) cam_h += 360.0f;

		while (cam_v > 360.0) cam_v -= 360.0f;
		while (cam_v < 0.0) cam_v += 360.0f;
		//***

		//Keyboard Input
		float pi = static_cast<float>(M_PI);
		float moveSpeed = 5.0f;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
			moveSpeed *= 2.0f;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			cam_z -= moveSpeed * dt * -sinf((cam_h*pi) / 180.0f);
			cam_x += moveSpeed * dt * cosf((cam_h*pi) / 180.0f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			cam_z += moveSpeed * dt * -sinf((cam_h*pi) / 180.0f);
			cam_x -= moveSpeed * dt * cosf((cam_h*pi) / 180.0f);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			cam_z += moveSpeed * dt * cosf((cam_h*pi) / 180.0f);
			cam_x -= moveSpeed * dt * sinf((cam_h*pi) / 180.0f);
			cam_y += moveSpeed * dt * sinf((cam_v*pi) / 180.0f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			cam_z -= moveSpeed * dt * cosf((cam_h*pi) / 180.0f);
			cam_x += moveSpeed * dt * sinf((cam_h*pi) / 180.0f);
			cam_y -= moveSpeed * dt * sinf((cam_v*pi) / 180.0f);
		}
		//***

		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glRotatef(cam_v, 1, 0, 0);
		glRotatef(cam_h, 0, 1, 0);
		glTranslatef(cam_x, cam_y, cam_z);

		// draw...
		display(shapes, materials, texfile);

		// end the current frame (internally swaps the front and back buffers)
		window.display();

		mouse_old = sf::Vector2f(sf::Mouse::getPosition());
		dt = clock.getElapsedTime().asSeconds();
	}
	//***

	return 0;
}
