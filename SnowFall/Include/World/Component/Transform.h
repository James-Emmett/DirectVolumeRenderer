#pragma once
#include "Component.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include <vector>
#include <memory>

enum class Space
{
	Self,
	World
};

class Transform : public Component<Transform>
{
private:
	std::shared_ptr<Transform>				m_Parent = nullptr;
	std::vector<std::shared_ptr<Transform>>	m_Children;

	Matrix4				m_Local = Matrix4::Identiy;
	Vector3				m_LocalPosition = Vector3(0, 0, 0);
	Quaternion			m_LocalRotation = Quaternion();
	Vector3				m_LocalScale = Vector3(1, 1, 1);

	mutable Matrix4		m_World = Matrix4::Identiy;
	mutable Matrix4		m_InvWorld = Matrix4::Identiy;
	mutable Vector3		m_WorldPosition = Vector3(0, 0, 0);
	mutable Quaternion	m_WorldRotation = Quaternion();
	mutable Vector3		m_WorldScale = Vector3(1, 1, 1);
	mutable Vector3		m_Euler = Vector3::Zero;
	mutable bool		m_HashChanged = false;

	bool				m_UniformScale = true;

public:
	//-------------World-------------
	//--Pos--
	Vector3 Position()const;
	void SetPosition(const Vector3& worldPosition);

	//--Rot--
	Quaternion Rotation()const;
	void SetRotation(const Quaternion& worldRotation);
	void SetRotation(const Vector3& worldRotation);

	//--Scale--
	Vector3 Scale()const;
	void SetScale(const Vector3& scale);

	//-------------Directions-------------
	Vector3 Right()const;
	Vector3 Up()const;
	Vector3 Forward()const;
	Vector3 LocalRight()const;
	Vector3 LocalUp()const;
	Vector3 LocalForward()const;
	Matrix4 World()const;
	Matrix4 WorldToLocalMatrix()const; // Its just the inverse

	//-------------Local-------------
	//--Pos--
	Vector3 LocalPosition()const;
	void SetLocalPosition(const Vector3& position);

	//--Rotation--
	Quaternion LocalRotation()const;
	void SetLocalRotation(const Quaternion& quat);
	void SetLocalRotation(const Vector3& euler);

	//--Scale--
	Vector3 LocalScale()const;
	void SetLocalScale(const Vector3& scale);

	//-------------Methods-------------
public:
	void Translate(const Vector3& translation, Space relative = Space::Self);
	void Rotate(const Quaternion& quat, Space relative = Space::Self);
	void Rotate(const Vector3& euler, Space relative = Space::Self);
	void LookAt(const Transform& target, Vector3 worldUp = Vector3::Up);
	void LookAt(const Vector3& position, Vector3 worldUp = Vector3::Up);

private:
	void HasChanged()const;
	void UpdateTransform()const; // Updates the world matrix stuff.

public:
	void OnGui();
};