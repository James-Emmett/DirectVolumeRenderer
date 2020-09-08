#pragma once
#include "World/Component/Transform.h"
#include "World/Scene.h"
#include "System/Types.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <typeindex>

class Entity
{

private:
	std::string m_Tag = "";
	std::string m_Name = "";
	Uint32		m_ID = -1;
	Scene* m_Scene = nullptr;
	bool		m_Active = true;
	bool		m_IsStatic = false;

	typedef std::unordered_map<ComponentType, std::shared_ptr<BaseComponent>> ComponentMap;
	typedef std::unordered_map<ComponentType, std::shared_ptr<BaseComponent>>::iterator ComponentMapIterator;

public:
	std::shared_ptr<Transform> m_Transform;
	std::unordered_map<ComponentType, std::shared_ptr<BaseComponent>> m_Components;

public:
	Entity();
	Entity(Scene* world, Uint32 id, std::string name = "");

public:
	Scene* GetScene()const;
	bool IsActive()const;
	void SetActive(bool value);
	void Update(float deltaTime);
	void Destroy();
	std::string GetName()const;
	void SetName(std::string& name);
	Uint32 GetID()const;

public:
	template<class T>
	std::shared_ptr<T> AddComponent();
	template<class T>
	std::shared_ptr<T> GetComponent();

public:
	bool operator==(const Entity& entity)const;
	bool operator!=(const Entity& entity)const;
};


template<class T>
inline std::shared_ptr<T> Entity::AddComponent()
{
	ComponentType type = T::TypeID;
	std::unordered_map<ComponentType, std::shared_ptr<BaseComponent>>::iterator itr = m_Components.find(type);
	if (itr != m_Components.end())
	{
		return std::static_pointer_cast<T>(itr->second);
	}

	std::shared_ptr<T> component = std::make_shared<T>();
	component->m_Entity = this;
	component->m_Transform = m_Transform;
	m_Components.insert(std::make_pair(type, component));
	m_Scene->OnEntityComponentChanged(component, ComponentChange::Add);
	return component;
}

template<class T>
inline std::shared_ptr<T> Entity::GetComponent()
{
	ComponentType type = T::TypeID;
	if (m_Components.count(type) != 0)
	{
		return std::static_pointer_cast<T>(m_Components[type]);
	}

	return std::shared_ptr<T>();
}