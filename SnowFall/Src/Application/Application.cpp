#include "Application/Application.h"
#include "Application/Game.h"

Game*			Application::game;
Time*			Application::time;
GraphicsDevice* Application::graphicsDevice;
ContentManager* Application::contentManager;
GameSettings*	Application::gameSettings;
Uint32 entityID = -1;

void Application::Quit()
{
	game->Exit();
}

void Application::Abort(std::string message)
{
	Window::ShowMessageBox(message);
	game->Exit();
}

Uint32 Application::NextEntityID()
{
	return entityID++;
}
