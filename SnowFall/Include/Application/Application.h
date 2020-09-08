// NOTE:
/*
    Static Application class contaning a bunch of important objects,
    basically the same as Unity's Application class. Please prefer dependency
    injection were possible though!!!

    I think settings should be rolled into this too?
*/

#pragma once
#include "System/Time.h"

class Game;
class GraphicsDevice;
class ContentManager;
class GameSettings;
class Application
{
public:
    static Game* game;
    static Time* time;
    static GraphicsDevice* graphicsDevice;
    static ContentManager* contentManager;
    static GameSettings*   gameSettings;

    static void Quit();
    static void Abort(std::string message);
    // Non serilizable
    static Uint32 NextEntityID();
};