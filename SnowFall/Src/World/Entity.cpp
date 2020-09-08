#include "World/Entity.h"
#include "World/Scene.h"

Entity::Entity() : m_Scene(nullptr)
{
	m_Transform = std::make_shared<Transform>();

	std::type_index index(typeid(Transform));
	ComponentType type = (ComponentType)index.hash_code();
	m_Components.emplace(type, m_Transform);
}

Entity::Entity(Scene* world, Uint32 id, std::string name) :
	m_Scene(world)
{
	m_ID = id;
	m_Name = name;
	m_Transform = std::make_shared<Transform>();

	std::type_index index(typeid(Transform));
	ComponentType type = (ComponentType)index.hash_code();
	m_Components.emplace(type, m_Transform);
}


Scene* Entity::GetScene() const
{
	return m_Scene;
}

bool Entity::IsActive() const
{
	return m_Active;
}

void Entity::SetActive(bool value)
{
	m_Active = value;
}

void Entity::Update(float deltaTime)
{
	for (ComponentMapIterator itr = m_Components.begin(); itr != m_Components.end(); itr++)
	{
		if (itr->second->m_Enabled)
		{
			itr->second->Update(deltaTime);
		}
	}
}

Uint32 Entity::GetID()const
{
	return m_ID;
}

std::string Entity::GetName()const
{
	return m_Name;
}

void Entity::SetName(std::string& name)
{
	m_Name = name;
}

bool Entity::operator==(const Entity& entity) const
{
	return m_ID == entity.m_ID;
}

bool Entity::operator!=(const Entity& entity) const
{
	return m_ID != entity.m_ID;
}