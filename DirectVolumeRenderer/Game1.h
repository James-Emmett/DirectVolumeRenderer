#pragma once
#include "Application/Game.h"
#include "World/Scene.h"
#include "FlyCamera.h"
#include "World/Renderer/ForwardRenderer.h"
#include "World/Component/MeshRenderer.h"
#include "World/Entity.h"
#include "GridRenderer.h"
#include "VolumeComponent.h"

class Game1 : public Game
{
	Scene m_Scene;
	ForwardRenderer m_Forwardrenderer;
	std::shared_ptr<FlyCamera> m_Camera;
	std::shared_ptr<GridRenderer> m_Grid;
	std::shared_ptr<VolumeComponent> m_VolumeComponent;

	// Temp Remove
	std::shared_ptr<MeshRenderer> m_GothicCabinet;

public:
	void Initialize();
	void LoadContent();
	void FixedUpdate();
	void Update(float deltaTime);
	void Draw();
	void OnGui();
	void OnResize();
	void ShutDown();
};