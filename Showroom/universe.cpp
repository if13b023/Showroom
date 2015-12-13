#include "universe.h"



universe::universe()
{
	sf::ContextSettings ctxSettings;
	ctxSettings.depthBits = 24;
	m_window.create(sf::VideoMode(800, 600), "Showroom OpenGL", sf::Style::Default, ctxSettings);
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

	if (!err.empty()) { // `err` may contain warning message.
		std::cout << err << std::endl;
	}
	if (!ret) {
		exit(1);
	}

	object_t tmp;
	tmp.shapes = shapes;
	tmp.materials = materials;

	for (int i = 0; i < materials.size(); ++i)
	{
		//if (materials.at(i).diffuse_texname.empty())
		//	break;

		texture_t tex;
		glGenTextures(1, &tex.id);

		sf::Image img;
		img.loadFromFile(materials.at(i).diffuse_texname);

		if (materials.at(i).diffuse_texname.empty())
		{
			tmp.textures.push_back(tex);
			continue;
		}
		
		tex.texture = img;

		glBindTexture(GL_TEXTURE_2D, tex.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, tex.texture.getSize().x, tex.texture.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.texture.getPixelsPtr());
		tmp.textures.push_back(tex);
	}

	objects.push_back(tmp);

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
	GLfloat lightPos[] = { 0.0f, 8.0f, 0.0f , 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
}

void universe::draw(tinyobj::shape_t & shape, tinyobj::material_t & mat)
{
	//float* diffuse = mat.diffuse;
	//mShininess[0] = mat.shininess;

	/*float diffuse[4];
	memcpy(diffuse, mat.diffuse, 3 * sizeof(float));
	diffuse[3] = mat.dissolve;*/

	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess);

	glTexCoordPointer(2, GL_FLOAT, 0, shape.mesh.texcoords.data());
	glVertexPointer(3, GL_FLOAT, 0, shape.mesh.positions.data());
	glNormalPointer(GL_FLOAT, 0, shape.mesh.normals.data());

	glDrawElements(GL_TRIANGLES, shape.mesh.indices.size(), GL_UNSIGNED_INT, shape.mesh.indices.data());
}

void universe::display(sceneobj& o, bool drawTrans)
{
	glPushMatrix();
	glTranslatef(o.position.x, o.position.y, o.position.z);
	glRotatef(o.rotation.y, 0, 1, 0);

	glPushMatrix();
	//use: glDrawElements
	for (int i = 0; i < o.object->shapes.size(); ++i)
	{
		if (drawTrans == false && o.object->materials.at(o.object->shapes.at(i).mesh.material_ids.at(0)).dissolve < 1.0f)
		{
			sceneobj tmpTrans;
			tmpTrans.object = o.object;
			tmpTrans.position = o.position;
			tmpTrans.rotation = sf::Vector3f(0, o.rotation.y, 0);

			sceneTrans.push_back(tmpTrans);

			continue;
		}

		//glBindTexture(GL_TEXTURE_2D, texturesIds[shapes[i].mesh.material_ids[0]]);
		glBindTexture(GL_TEXTURE_2D, o.object->textures.at(o.object->shapes.at(i).mesh.material_ids.at(0)).id);

		draw(o.object->shapes.at(i), o.object->materials.at(o.object->shapes.at(i).mesh.material_ids.at(0)));
	}

	glPopMatrix();

	glPopMatrix();
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
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
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

	//Build the scene
	sceneobj showroom;
	showroom.object = &objects.at(0);
	showroom.position = showroom.rotation = sf::Vector3f(0, 0, 0);

	scene.push_back(showroom);

	sceneobj abarth;
	abarth.object = &objects.at(1);
	abarth.position = sf::Vector3f(0, 1.3f, -15.0f);
	abarth.rotation = sf::Vector3f(0, 0, 0);
	for (int i = 0; i <= 3; ++i)
	{
		scene.push_back(abarth);
		abarth.position.z += 15.0f;
	}
	//***

	init();

	//loop
	int cnt = 0;
	int meshSwitch = 0;
	bool quit = false;
	bool showInfo = true;

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
				else if (event.key.code == sf::Keyboard::Num3)
					showInfo = !showInfo;
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
		display(scene.at(0), true);	//Draw Showroom

		float dist = 0;
		for (int i = 1; i < scene.size(); ++i)
		{
			dist = distance(m_camPos, scene.at(i).position);

			meshSwitch = 3;
			if (dist > 15.0f)
				meshSwitch = 2;
			if (dist > 30.0f)
				meshSwitch = 1;

			display(scene.at(i));
			/*if (meshSwitch == 0)
				display(objects[1], objects_mat[1], abarths[i], 0);
			else if (meshSwitch == 1)
				display(objects[1], objects_mat[1], abarths[i], 45.0f);
			else
				display(objects[1], objects_mat[1], abarths[i], 90.0f);*/
			
			//objects[meshSwitch].position = abarths[i];
			//display(objects[meshSwitch], false);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
		//drawTransparent();
		for (int i = 0; i < sceneTrans.size(); ++i)
			display(sceneTrans.at(i), true);
		glDisable(GL_BLEND);

		//end the current frame (internally swaps the front and back buffers)
		m_window.display();

		sceneTrans.clear();
		/*transparents_mat.clear();
		transparent_pos.clear();
		transparent_rot.clear();*/

		if (cnt == 100 && showInfo)
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
