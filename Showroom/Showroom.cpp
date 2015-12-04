// Showroom.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string.h>

#include <tiny_obj_loader.h>

#include <GL/glut.h>
#include <GL/GL.h>
#include <GL/GLU.h>

int window;
float	inc = 1.0f,
		rot = 0.0f;

float	main_z = 10.0f,
		main_x = 0.0f;

bool toggleColor = false;

std::vector<tinyobj::shape_t> shapes;
std::vector<tinyobj::material_t> materials;

GLfloat redDiffuseMaterial[] = { 1.0, 0.0, 0.0 }; //set the material to red
GLfloat whiteSpecularMaterial[] = { 1.0, 1.0, 1.0 }; //set the material to white
GLfloat greenEmissiveMaterial[] = { 0.0, 1.0, 0.0 }; //set the material to green
GLfloat whiteSpecularLight[] = { 1.0, 1.0, 1.0 }; //set the light specular to white
GLfloat blackAmbientLight[] = { 0.0, 0.0, 0.0 }; //set the light ambient to black
GLfloat whiteDiffuseLight[] = { 1.0, 1.0, 1.0 }; //set the diffuse light to white
GLfloat blankMaterial[] = { 0.0, 0.0, 0.0 }; //set the diffuselight to white
GLfloat mShininess[] = { 64 }; //set the shininess of the material

void resize(int width, int height)
{
	// prevent division by zero
	if (height == 0) { height = 1; }

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)width / (float)height, 0.1f, 500.0f);
	glMatrixMode(GL_MODELVIEW);
}

void keyPressed(unsigned char key, int x, int y)
{
	//usleep(100);
	//std::cout << key << std::endl;
	switch (key) {
	case 27:
		glutDestroyWindow(window);
		exit(0);
		break;

	case 'w':
		main_z += 1.0f;
		break;

	case 's':
		main_z -= 1.0f;
		break;

	case 'd':
		main_x -= 1.0f;
		break;

	case 'a':
		main_x += 1.0f;
		break;

	case 't':
		if (toggleColor)
			glMaterialfv(GL_FRONT, GL_EMISSION, blankMaterial);
		else
			glMaterialfv(GL_FRONT, GL_EMISSION, greenEmissiveMaterial);
		toggleColor = !toggleColor;
		break;
	}
}

static void specialKeyPressed(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_RIGHT:
		inc += 2.0;
		break;
	case GLUT_KEY_LEFT:
		inc -= 2.0;
		break;
	}
}

void light(void)
{
	GLfloat lightPos[] = { 10.0f, 0.0f, 0.0f , 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, blackAmbientLight);
}

void display()
{
	GLfloat material[] = { 1.0, 0.0, 0.0 };
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, material);

	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	gluLookAt(sinf(rot) * 10, 0, cosf(rot) * 10, 0, 0, 0, 0, 1, 0);

	light();

	//glRotatef(inc, 0, 1, 0);
	//glTranslatef(main_x, 0, main_z);

	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glMaterialfv(GL_FRONT, GL_EMISSION, blankMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, redDiffuseMaterial);
	//glTranslatef(0.0, 0.0, 0.0);
	glRotatef(90, 1, 0, 0);
	glutWireSphere(400.0, 32 * 2, 16 * 2);
	glPopAttrib();
	glPopMatrix();

	/*glPushMatrix();
	glTranslatef(0.0, 0.0, -70.0);
	glutWireCube(20.0);
	glPopMatrix();*/

	glPushMatrix();

	glTranslatef(0, 0, -5.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

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
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 5);
	glutSolidTeapot(2);
	glPopMatrix();

	glutSwapBuffers();

	int err = GL_NO_ERROR;
	if ((err = glGetError()) != GL_NO_ERROR)
		std::cout << "Error: " << err << std::endl;

	rot += 0.01f;
}

void init(int width, int height)
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
	resize(width, height);
}

void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000.0 / 60.0, &timer, 1);
}

int main(int argc, char** argv)
{
	std::string inputfile = "suzanne.obj";
	std::string inputdir = "../";

	//std::cin >> inputdir;
	//std::cin >> inputfile;

	//std::vector<tinyobj::shape_t> shapes;
	//std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(shapes, materials, err, (inputdir + inputfile).c_str(), inputdir.c_str());

	if (!err.empty()) { // `err` may contain warning message.
		std::cout << err << std::endl;
	}

	if (!ret) {
		exit(1);
	}

	std::cout << "# of shapes    : " << shapes.size() << std::endl;
	std::cout << "# of materials : " << materials.size() << std::endl;

	/*for (size_t i = 0; i < shapes.size(); i++) {
		printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
		printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
		printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
		assert((shapes[i].mesh.indices.size() % 3) == 0);
		for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
			printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3 * f + 0], shapes[i].mesh.indices[3 * f + 1], shapes[i].mesh.indices[3 * f + 2], shapes[i].mesh.material_ids[f]);
		}

		printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
		assert((shapes[i].mesh.positions.size() % 3) == 0);
		for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
			printf("  v[%ld] = (%f, %f, %f)\n", v,
				shapes[i].mesh.positions[3 * v + 0],
				shapes[i].mesh.positions[3 * v + 1],
				shapes[i].mesh.positions[3 * v + 2]);
		}
	}

	for (size_t i = 0; i < materials.size(); i++) {
		printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
		printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
		printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
		printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
		printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
		printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
		printf("  material.Ns = %f\n", materials[i].shininess);
		printf("  material.Ni = %f\n", materials[i].ior);
		printf("  material.dissolve = %f\n", materials[i].dissolve);
		printf("  material.illum = %d\n", materials[i].illum);
		printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
		printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
		printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
		printf("  material.map_Ns = %s\n", materials[i].specular_highlight_texname.c_str());
		std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
		std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
		for (; it != itEnd; it++) {
			printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
		}
		printf("\n");
	}*/

	//shape = shapes[0].mesh.indices.data();
	//shape_size = shapes[0].mesh.indices.size();

	//OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	window = glutCreateWindow("Showroom");
	glutDisplayFunc(display);
	//glutIdleFunc(&display); 
	glutTimerFunc(1000.0 / 60.0, &timer, 1);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialKeyPressed);
	init(640, 480);

	light();

	glMaterialfv(GL_FRONT, GL_DIFFUSE, redDiffuseMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecularMaterial);
	glMaterialfv(GL_FRONT, GL_SHININESS, mShininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, blankMaterial);

	glutMainLoop();

	return 0;
}

