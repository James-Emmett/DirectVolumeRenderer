//Note:
/*
	Snowfall currently uses the old fashioned Component Entity Architecture like unity, i spent way too much
	time thinking about implementing soloutions to ECS that never really panned out to be worth while.

	If i have time in the future, change entities to use a Sparse Set with an EntityID -> Handle Map,
	this will keep entities packed but allow access from an EntityID. Components as well should be
	created inside ComponentPools that also use a Sparse Set, this keeps them contigious in memory
	but still too allows Access via EntityID. Consider adding a ComponentHandle too simplify look up
	in the ComponentManager etc.
*/

#pragma once
#include "Renderer/RenderSettings.h"
#include "World/Component/RenderComponent.h"
#include "Graphics/GraphicsDevice.h"
#include "World/Component/Camera.h"
#include <unordered_map>
#include <vector>
#include <memory>

enum class ComponentChange { Add, Remove };

class RenderComponent;
class BaseComponent;
class Entity;
class Scene
{
private:
	typedef std::unordered_map<Uint32, std::unique_ptr<Entity>> EntityMap;
	typedef std::unordered_map<Uint32, std::unique_ptr<Entity>>::iterator EntitiyMap_Iterator;
	typedef std::unordered_map<Uint32, std::unique_ptr<Entity>>::const_iterator Const_EntitiyMap_Iterator;

	EntityMap m_EntityMap;
	GraphicsDevice* m_GraphicsDevice;
public:
	RenderSettings m_RenderSettings;

	//--List of Core Components--
	std::vector<std::shared_ptr<Camera>>			m_CameraList;
	//std::vector<std::shared_ptr<Light>>			m_LightList;
	std::vector<std::shared_ptr<RenderComponent>>	m_RenderList;

public:
	void Initialize(GraphicsDevice* device);
	void Update(float deltaTime);
	void OnGui();
	Entity* CreateEntity(Uint32 id);
	Entity* GetEntity(Uint32 entityID)const;
	void Clear();
	void AddRenderComponent(std::shared_ptr<RenderComponent> component);
	void AddCameraComponent(std::shared_ptr<Camera> component);
	void RemoveRenderComponent(std::shared_ptr<RenderComponent> component);
	void RemoveCameraComponent(std::shared_ptr<Camera> component);

	void OnEntityComponentChanged(std::shared_ptr<BaseComponent> component, ComponentChange changeType);
};