#include "World/Scene.h"
#include "World/Entity.h"
#include "Application/Application.h"
#include "Content/ContentManager.h"
#include <World\Renderer\PostProcess\PostProcessor.h>

void Scene::Initialize(GraphicsDevice* device)
{
	m_GraphicsDevice = device;
	m_RenderSettings.m_SkyBox.Initialize(m_GraphicsDevice, Application::contentManager);
}

void Scene::Update(float deltaTime)
{
	for (EntitiyMap_Iterator itr = m_EntityMap.begin(); itr != m_EntityMap.end(); itr++)
	{
		if (itr->second->IsActive())
		{
			itr->second->Update(deltaTime);
		}
	}
}

void Scene::OnGui()
{
}

Entity* Scene::CreateEntity(Uint32 id)
{
	m_EntityMap.emplace(id, std::make_unique<Entity>(this, id, "default"));
	return m_EntityMap[id].get();
}

Entity* Scene::GetEntity(Uint32 entityID)const
{
	Const_EntitiyMap_Iterator itr = m_EntityMap.find(entityID);
	if (itr != m_EntityMap.end())
	{
		return itr->second.get();
	}

	return nullptr;
}

void Scene::Clear()
{
	m_RenderList.clear();
	m_CameraList.clear();
	m_EntityMap.clear();
}

void Scene::AddRenderComponent(std::shared_ptr<RenderComponent> component)
{
	std::vector<std::shared_ptr<RenderComponent>>::iterator itr = std::find(m_RenderList.begin(), m_RenderList.end(), component);
	if (itr == m_RenderList.end())
	{
		m_RenderList.push_back(std::static_pointer_cast<RenderComponent>(component));
	}
}

void Scene::AddCameraComponent(std::shared_ptr<Camera> component)
{
	std::vector<std::shared_ptr<Camera>>::iterator itr = std::find(m_CameraList.begin(), m_CameraList.end(), component);
	if (itr == m_CameraList.end())
	{
		m_CameraList.push_back(std::static_pointer_cast<Camera>(component));
	}
}

void Scene::RemoveRenderComponent(std::shared_ptr<RenderComponent> component)
{
	std::vector<std::shared_ptr<RenderComponent>>::iterator itr = std::find(m_RenderList.begin(), m_RenderList.end(), component);
	if (itr != m_RenderList.end())
	{
		m_RenderList.erase(itr);
	}
}

void Scene::RemoveCameraComponent(std::shared_ptr<Camera> component)
{
	std::vector<std::shared_ptr<Camera>>::iterator itr = std::find(m_CameraList.begin(), m_CameraList.end(), component);
	if (itr != m_CameraList.end())
	{
		m_CameraList.erase(itr);
	}
}

void Scene::AddPostProcessor(std::shared_ptr<PostProcessor> postprocess)
{
	m_PostProcessors.push_back(postprocess);
	std::sort(m_PostProcessors.begin(), m_PostProcessors.end(), std::less<std::shared_ptr<PostProcessor>>());
}

void Scene::RemovePostProcessor(std::shared_ptr<PostProcessor> postProcess)
{
	for (size_t i = 0; i < m_PostProcessors.size(); i++)
	{
		if (m_PostProcessors[i] == postProcess)
		{
			m_PostProcessors.erase(m_PostProcessors.begin() + i);
		}
	}
}

void Scene::OnEntityComponentChanged(std::shared_ptr<BaseComponent> component, ComponentChange changeType)
{
	ComponentType type = component->GetType();
	if (type == RenderComponent::TypeID)
	{
		if (changeType == ComponentChange::Add)
		{
			AddRenderComponent(std::static_pointer_cast<RenderComponent>(component));
		}
		else
		{
			RemoveRenderComponent(std::static_pointer_cast<RenderComponent>(component));
		}
	}
	else if (type == Camera::TypeID)
	{
		if (changeType == ComponentChange::Add)
		{
			AddCameraComponent(std::static_pointer_cast<Camera>(component));
		}
		else
		{
			RemoveCameraComponent(std::static_pointer_cast<Camera>(component));
		}
	}
}
