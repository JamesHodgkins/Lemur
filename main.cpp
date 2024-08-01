#include "classes/application.h"

int main(int, char**)
{
	Lemur::Application* app = Lemur::Application::getInstance(); 

	// Main loop
	while (app->isRunning())
	{
		app->update();
	}

	// Release app
	delete app;

	return 0;
}