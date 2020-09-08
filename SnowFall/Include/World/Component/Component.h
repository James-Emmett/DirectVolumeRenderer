#pragma once
#include "System/Types.h"
#include <memory>

typedef Uint32 ComponentType;

class Entity;
class Transform;
class BaseComponent
{
public:
	Entity* m_Entity;
	std::shared_ptr<Transform> m_Transform;
	bool   m_Enabled = true;

public:
	BaseComponent();
	virtual ~BaseComponent() = default;
	virtual void Update(float deltaTime) {};
	virtual void OnGui() {};
	virtual ComponentType GetType(void) const = 0;

	static Uint32 componentID;
};

template<class T>
class Component : public BaseComponent
{
public:
	static const ComponentType TypeID;

	ComponentType GetType(void)const
	{
		return TypeID;
	}
};

template<typename T>
const ComponentType Component<T>::TypeID = Component::componentID++;