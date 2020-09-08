//Note:
/*
	Enums and structs for renderers
*/

#pragma once
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "System/Types.h"
#include "System/Assert.h"
#include <algorithm>
#include <memory>
#include <vector>

enum class TextureDefaults
{
	Skybox,
	Irradiance,
	SpecularHDR,
	SpecularLUT,
	Count
};

// Render Constant buffers
enum class UniformTypes
{
	Frame,
	Camera,
	Object,
	Count
};

// Alot of defaults
struct FrameConstBuffer
{
	float m_Width;
	float m_Height;
	float m_Time;
	float m_SinTime;
	float m_CostTime;
	float m_DeltaTime;

	// Should find 2 floats i could put here
	float _Pad_01;
	float _Pad_02;
};

// Constant buffer for camera data
struct CameraConstBuffer
{
	Matrix4 m_View;
	Matrix4 m_Projection;
	Matrix4 m_ViewProjection;

	Matrix4 m_InvView;
	Matrix4 m_InvProjection;
	Matrix4 m_InvViewProjection;

	Vector3 m_CameraPosition;
	float   m_FocalLength = 0.0f;
};

// Const buffer for object data, world, inv world etc.
// may be removed in future.
struct ObjectConstBuffer
{
	Matrix4 m_World;
	Matrix4 m_InvWorld;
	Matrix4 m_WorldViewProj;
	Matrix4 m_NormalMatrix;
	Vector3 m_CameraPosLocal;
	float   m_pad_01 = 0.0f;
};

enum class RenderSortMode
{
	FrontToBack,
	BackToFront
};

// Unoptomized, make data oriented also make all states fixed types in engine for future!
// Also use a sort Key instead of just distance???
class RenderComponent;
struct RenderItem
{
	std::shared_ptr<RenderComponent> m_Renderer;
	float m_Distance = 0;

	bool operator < (const RenderItem& item) const
	{
		return m_Distance < item.m_Distance;
	}

	bool operator > (const RenderItem& item) const
	{
		return m_Distance > item.m_Distance;
	}
};

class Camera;
struct RenderQueue
{
	std::shared_ptr<Camera> m_Camera;
	std::vector<RenderItem> m_RenderItems;

	// Change this to use a frame allocator
	RenderItem& Alocate()
	{
		m_RenderItems.push_back(RenderItem());
		return m_RenderItems[m_RenderItems.size() - 1];
	}

	void Sort(RenderSortMode mode)
	{
		// Opaques use this
		if (mode == RenderSortMode::FrontToBack)
		{
			std::sort(m_RenderItems.begin(), m_RenderItems.end());
		}
		else if (mode == RenderSortMode::BackToFront)
		{
			std::sort(m_RenderItems.begin(), m_RenderItems.end());
		}
	}

	Uint32 Size()const
	{
		return (Uint32)m_RenderItems.size();
	}

	const RenderItem& operator[](Uint32 index)const
	{
		assert(index < m_RenderItems.size());
		return m_RenderItems[index];
	}

	void Clear()
	{
		m_RenderItems.clear();
	}
};