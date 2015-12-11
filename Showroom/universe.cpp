#include "universe.h"



universe::universe()
{
	m_window.create(sf::VideoMode(800, 600), "Showroom OpenGL", sf::Style::Default, sf::ContextSettings(32));
}

universe::~universe()
{
	m_window.close();
}

bool universe::addObject(char* dir, char* file)
{
	bool ret = false;
	std::string err;

	char* comp = new char[strlen(dir) + strlen(file) + 1];
	memcpy(comp, dir, strlen(dir));
	memcpy(comp + strlen(dir), file, strlen(file));
	comp[strlen(dir) + strlen(file)] = '\0';

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	ret = tinyobj::LoadObj(shapes, materials, err, comp, dir);
	objects.push_back(shapes);
	objects_mat.push_back(materials);
	if (!err.empty()) { // `err` may contain warning message.
		std::cout << err << std::endl;
	}
	if (!ret) {
		exit(1);
	}

	return ret;
}

void universe::resize()
{
	int height = m_window.getSize().y;
	int width = m_window.getSize().x;
	// prevent division by zero
	if (m_window.getSize().y == 0) { height = 1; }

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = static_cast<float>(m_window.getSize().x) / m_window.getSize().y;
	glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 300.f);
	glMatrixMode(GL_MODELVIEW);
}

void universe::light()
{
	GLfloat lightPos[] = { 0.0f, 10.0f, 0.0f , 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
}

void universe::draw(tinyobj::shape_t & shape, tinyobj::material_t & mat)
{
	float* diffuse = mat.diffuse;
	mShininess[0] = mat.shininess;

	//material[0] = diffuse[0];
	//material[1] = diffuse[1];
	//material[2] = diffuse[2];

	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
	glMaterialfv(GL_FRONT, GL_SHININESS, mShininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

	glTexCoordPointer(2, GL_FLOAT, 0, shape.mesh.texcoords.data());
	glVertexPointer(3, GL_FLOAT, 0, shape.mesh.positions.data());
	glNormalPointer(GL_FLOAT, 0, shape.mesh.normals.data());

	glDrawElements(GL_TRIANGLES, shape.mesh.indices.size(), GL_UNSIGNED_INT, shape.mesh.indices.data());
}

void universe::display(std::vector<tinyobj::shape_t>& shapes, std::vector<tinyobj::material_t>& materials, sf::Vector3f & pos, float yrot)
{
	GLfloat material[] = { 1.0, 0.0, 1.0 };
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);
	glRotatef(yrot, 0, 1, 0);

	glPushMatrix();

	//use: glDrawElements
	for (int i = 0; i < shapes.size(); ++i)
	{
		/*float* diffuse = materials[shapes[i].mesh.material_ids[0]].diffuse;
		mShininess[0] = materials[shapes[i].mesh.material_ids[0]].shininess;
		//float diffuse[] = { 0, 0, 1.0 };
		material[0] = diffuse[0];
		material[1] = diffuse[1];
		material[2] = diffuse[2];

		glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
		glMaterialfv(GL_FRONT, GL_SHININESS, mShininess);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, material);

		glTexCoordPointer(2, GL_FLOAT, 0, shapes[i].mesh.texcoords.data());
		glVertexPointer(3, GL_FLOAT, 0, shapes[i].mesh.positions.data());
		glNormalPointer(GL_FLOAT, 0, shapes[i].mesh.normals.data());

		glDrawElements(GL_TRIANGLES, shapes[i].mesh.indices.size(), GL_UNSIGNED_INT, shapes[i].mesh.indices.data());*/
		draw(shapes[i], materials[shapes[i].mesh.material_ids[0]]);
	}

	//glVertexPointer(3, GL_FLOAT, 0, shapes[0].mesh.positions.data());
	//glDrawElements(GL_TRIANGLES, shapes[0].mesh.indices.size(), GL_UNSIGNED_INT, shapes[0].mesh.indices.data());

	//use: glDrawArrays
	//glVertexPointer(3, GL_FLOAT, 0, shapes[0].mesh.positions.data());
	//glDrawArrays(GL_TRIANGLES, 0, shapes[0].mesh.positions.size());

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();

	glPopMatrix();

	//glutSwapBuffers();

	/*int err = GL_NO_ERROR;
	if ((err = glGetError()) != GL_NO_ERROR)
	std::cout << "Error: " << err << std::endl;*/
}

void universe::init()
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
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	resize();
}

float universe::distance(sf::Vector3f& a, sf::Vector3f& b)
{
	sf::Vector3f diff(a - b);

	float dist = sqrt(pow(diff.x, 2) + pow(diff.y, 2) + pow(diff.z, 2));
	return dist;
}

void universe::run()
{
	sf::Vector2f mouse_old(0, 0);

	sf::Vector3f tmppos(0, 1.3f, -15.0);
	for (int i = 0; i <= 3; ++i)
	{
		tmppos.z += 15.0f;
		abarths.push_back(tmppos);
	}

	init();

	//loop
	int cnt = 0;
	int meshSwitch = 0;
	bool quit = false;

	while (!quit)
	{
		clock.restart();

		sf::Event event;
		while (m_window.pollEvent(event))
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
				resize();
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
					quit = true;
				else if (event.key.code == sf::Keyboard::Num1)
					std::cout << m_camRot.y << std::endl;
				else if (event.key.code == sf::Keyboard::Num2)
				{
					if (meshSwitch < 3)
						++meshSwitch;
					else
						meshSwitch = 0;
					std::cout << meshSwitch << std::endl;
				}
			}
		}

		//Mouse Input
		float sens = 3.0f;
		//sf::Vector2f mouse_diff = mouse_old - sf::Vector2f(sf::Mouse::getPosition());
		sf::Vector2f mouse_diff = sf::Vector2f(sf::Mouse::getPosition());
		m_camRot.y = (mouse_diff.x / sf::VideoMode::getDesktopMode().width) * 360.0f * sens;
		m_camRot.x = (mouse_diff.y / sf::VideoMode::getDesktopMode().height) * 360.0f * sens;

		while (m_camRot.y > 360.0) m_camRot.y -= 360.0f;
		while (m_camRot.y < 0.0) m_camRot.y += 360.0f;

		while (m_camRot.x > 360.0) m_camRot.x -= 360.0f;
		while (m_camRot.x < 0.0) m_camRot.x += 360.0f;

		if (m_camRot.x < 270.0f && m_camRot.x > 180.0f)
			m_camRot.x = 271.0f;
		else if (m_camRot.x > 90.0f && m_camRot.x < 180.0f)
			m_camRot.x = 89.0f;
		//***

		//Keyboard Input
		float pi = static_cast<float>(M_PI);
		float moveSpeed = 5.0f;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
			moveSpeed *= 2.0f;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			m_camPos.z -= moveSpeed * dt * -sinf((m_camRot.y*pi) / 180.0f);
			m_camPos.x += moveSpeed * dt * cosf((m_camRot.y*pi) / 180.0f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			m_camPos.z += moveSpeed * dt * -sinf((m_camRot.y*pi) / 180.0f);
			m_camPos.x -= moveSpeed * dt * cosf((m_camRot.y*pi) / 180.0f);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			m_camPos.z += moveSpeed * dt * cosf((m_camRot.y*pi) / 180.0f);
			m_camPos.x -= moveSpeed * dt * sinf((m_camRot.y*pi) / 180.0f);
			m_camPos.y += moveSpeed * dt * sinf((m_camRot.x*pi) / 180.0f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			m_camPos.z -= moveSpeed * dt * cosf((m_camRot.y*pi) / 180.0f);
			m_camPos.x += moveSpeed * dt * sinf((m_camRot.y*pi) / 180.0f);
			m_camPos.y -= moveSpeed * dt * sinf((m_camRot.x*pi) / 180.0f);
		}
		//***

		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glRotatef(m_camRot.x, 1, 0, 0);
		glRotatef(m_camRot.y, 0, 1, 0);
		glTranslatef(-m_camPos.x, -m_camPos.y, -m_camPos.z);

		// draw...
		light();
		display(objects[0], objects_mat[0], sf::Vector3f(0,0,0), 0);

		float dist;
		for (int i = 0; i < abarths.size(); ++i)
		{
			dist = distance(m_camPos, abarths[i]);

			meshSwitch = 2;
			if (dist > 15.0f)
				meshSwitch = 1;
			if (dist > 30.0f)
				meshSwitch = 0;

			/*if (meshSwitch == 0)
				display(objects[1], objects_mat[1], abarths[i], 0);
			else if (meshSwitch == 1)
				display(objects[1], objects_mat[1], abarths[i], 45.0f);
			else
				display(objects[1], objects_mat[1], abarths[i], 90.0f);*/

			if (meshSwitch == 0)
				display(objects[1], objects_mat[1], abarths[i], 0);
			else if (meshSwitch == 1)
				display(objects[2], objects_mat[2], abarths[i], 0);
			else
				display(objects[3], objects_mat[3], abarths[i], 0);
		}
		//end the current frame (internally swaps the front and back buffers)
		m_window.display();

		if (cnt == 100)
		{
			//std::cout << cam_x << "\t" << cam_y << "\t" << cam_z << "\t" << cam_h << "\t" << cam_v << std::endl;
			std::cout << 1 / dt << " : " << dist  << ": " << meshSwitch << std::endl;
			cnt = 0;
		}
		cnt++;

		mouse_old = sf::Vector2f(sf::Mouse::getPosition());
		dt = clock.getElapsedTime().asSeconds();
	}
}
