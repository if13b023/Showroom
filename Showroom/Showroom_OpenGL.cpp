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

#include "universe.h"

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
		std::cout << "Argument: " << argv[i] << std::endl;

	universe u;

	char* dir = "../Models/";

	u.addObject(dir, "Showroom2_newtex.obj");
	u.addObject(dir, "fiat500blandswap_low.obj");
	//u.addObject(dir, "fiat500blandswap_mid.obj");
	//u.addObject(dir, "fiat500blandswap_high.obj");

	u.run();

	return 0;
}
