#include "World/Component/Camera.h"
#include "World/Component/Transform.h"
#include "Math/Vector2.h"

Camera::Camera()
{
	SetCamera(1280, 720, 70, 0.1f, 1000.0f, Projection::Perspective);
}

Camera::~Camera()
{
}

void Camera::SetCamera(Uint32 width, Uint32 height, float fov, float nearPlane, float farPlane, Projection mode)
{
	SetFOV(fov);
	SetAspectRatio((float)width / height);
	SetWidth(width);
	SetHeight(height);
	SetNearPlane(nearPlane);
	SetFarPlane(farPlane);
	m_ProjectionMode = Projection::Perspective;
}

void Camera::SetNearPlane(float nearPlane)
{
	m_NearPlane = nearPlane;
	m_ProjectDrity = true;
}

float Camera::GetNearPlane() const
{
	return m_NearPlane;
}

void Camera::SetFarPlane(float farPlane)
{
	m_FarPlane = farPlane;
	m_ProjectDrity = true;
}

float Camera::GetFarPlane() const
{
	return m_FarPlane;
}

void Camera::SetAspectRatio(float aspect)
{
	m_AspectRatio = aspect;
	m_ProjectDrity = true;
}

float Camera::GetAspect() const
{
	return m_AspectRatio;
}

void Camera::SetFOV(float fov)
{
	m_FOV = Mathf::DEG_TO_RAD * fov;
	m_ProjectDrity = true;
}

float Camera::GetFOV() const
{
	return m_FOV;
}

float Camera::GetNearPlaneWidth() const
{
	return m_AspectRatio * (2.0f * (Mathf::Tan(m_FOV * 0.5f) * m_NearPlane));
}

float Camera::GetNearPlaneHeight() const
{
	return 2.0f * (Mathf::Tan(m_FOV * 0.5f) * m_NearPlane);
}

float Camera::GetFarPlaneWidth() const
{
	return m_AspectRatio * (2.0f * (Mathf::Tan(m_FOV * 0.5f) * m_FarPlane));
}

float Camera::GetFarPlaneHeight() const
{
	return 2.0f * (Mathf::Tan(m_FOV * 0.5f) * m_FarPlane);
}

float Camera::GetFocalLength() const
{
	// Not sure about this.
	return Mathf::Abs(1.0f / Mathf::Tan(m_FOV * 0.5f));
}

void Camera::SetWidth(Uint32 width)
{
	m_Width = width;
	m_ProjectDrity = true;
}

Uint32 Camera::GetWidth() const
{
	return m_Width;
}

void Camera::SetHeight(Uint32 height)
{
	m_Height = height;
	m_ProjectDrity = true;
}

Uint32 Camera::GetHeight() const
{
	return m_Height;
}

Matrix4 Camera::GetView() const
{
	return m_View;
}

void Camera::SetView(Matrix4 view)
{
	m_View = view;
	m_InvView = Matrix4::Inverse(view);
}

Matrix4 Camera::GetInvView() const
{
	return m_InvView;
}

Matrix4 Camera::GetProjection()const
{
	return m_Projection;
}

void Camera::SetProjection(Matrix4 proj)
{
	m_Projection = m_Projection;
}

Matrix4 Camera::GetInvProjection() const
{
	return m_InvProjection;
}

Matrix4 Camera::GetViewProjection() const
{
	return m_Projection * m_View;
}

void Camera::LookAt(Vector3 target)
{
	m_Transform->LookAt(target);
}

Vector3 Camera::ViewPortToWorldPoint(const Vector3& position) const
{
	return m_InvView.TransformPoint(position);
}

Vector3 Camera::GetPosition() const
{
	return m_Transform->Position();
}

void Camera::Move(const Vector3& translate)
{
	if (Mathf::IsZero(translate.LengthSquared())) { return; }
	m_Transform->Translate(translate);
	m_ViewDirty = true;
}

void Camera::Rotation(const Vector3& euler)
{
	m_Transform->Rotate(euler);
	m_ViewDirty = true;
}

void Camera::Rotate(float yaw, float pitch)
{
	// Rule, pitch locally, yaw globally, prevents converion of yaw,pitch into roll
	// Rotation is  hack to fix the roll drift :/
	m_Transform->Rotate(Vector3(pitch, 0, 0), Space::Self);
	m_Transform->Rotate(Vector3(0, yaw, 0), Space::World);
	m_ViewDirty = true;
}

void Camera::Update(float deltaTime)
{
	if (m_ViewDirty)
	{
		SetView(Matrix4::LookAt(m_Transform->Position(), m_Transform->Position() + m_Transform->Forward(), m_Transform->Up()));
	}

	m_ViewDirty = false;

	if (m_ProjectDrity)
	{
		if (m_ProjectionMode == Projection::Orthographic)
		{
			m_Projection = Matrix4::Orthographic((int)m_Width, (int)m_Height, m_NearPlane, m_FarPlane);
		}
		else
		{
			m_Projection = Matrix4::PerspectiveFov(m_FOV, (float)m_Width / m_Height, m_NearPlane, m_FarPlane);
		}
	}
}

void Camera::OnGui()
{
	// Does Nothing.
}