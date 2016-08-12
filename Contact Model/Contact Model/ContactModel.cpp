// Falling Boxes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FreeGLUTCallbacks.h"
#include "ContactModelApp.h"


int main(int argc, char **argv)
{
	//FallingBoxesApplication fallingBoxes(PERSPECTIVE);
	//FallingBoxesApplication fallingBoxes(ORTHOGRAPHIC);
	ContactModelApp contactModelApp(PERSPECTIVE);
	return glutmain(argc, argv, 1024, 768, "Contact Model", &contactModelApp);
}
