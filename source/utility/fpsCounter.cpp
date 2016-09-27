#include <stdio.h>

#include "glfw3.h"

static double lastTime = glfwGetTime();	    //Last time checked
static int frameCount = 0;					//Frames since last second

void logFPS()
{
	//Get current time and increment frame count
	double currentTime = glfwGetTime();
	frameCount++;

	//If it has been one second
	if (currentTime - lastTime >= 1.0)
	{
		//Print frame render time and reset counters
		printf("%f ms/frame\n", 1000.0 / double(frameCount));
		frameCount = 0;
		lastTime += 1.0;
	}
}