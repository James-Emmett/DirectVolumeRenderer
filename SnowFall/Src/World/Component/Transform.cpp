#include "World/Component/Transform.h"
#include "UI/ImGui_Interface.h"

Vector3 Transform::Position() const
{
    if (m_HashChanged)
    {
        UpdateTransform();
    }

    return m_WorldPosition;
}

void Transform::SetPosition(const Vector3& worldPosition)
{
    if (m_Parent == nullptr)
    {
        SetLocalPosition(worldPosition);
    }
    else
    {
        //Transform the world position relative to the parents space
        SetLocalPosition(Matrix4::Inverse(m_Parent->World()).Transform(worldPosition));
    }
}

Quaternion Transform::Rotation() const
{
    if (m_HashChanged)
    {
        UpdateTransform();
    }

    return m_WorldRotation;
}

void Transform::SetRotation(const Quaternion& worldRotation)
{
    if (!m_Parent)
    {
        SetLocalRotation(worldRotation);
    }
    else
    {
        // Transform the world position relative to the parents space
        SetLocalRotation(Quaternion::Inverse(m_Parent->Rotation()) * worldRotation);
    }
}

void Transform::SetRotation(const Vector3& worldRotation)
{
    if (!m_Parent)
    {
        SetLocalRotation(worldRotation);
    }
    else
    {
        // Transform world rotation into local space of heirarchy, just uses the quat version.
        SetLocalRotation(Quaternion::Inverse(m_Parent->Rotation()) * Quaternion::Euler(worldRotation));
    }
}

Vector3 Transform::Scale() const
{
    if (m_HashChanged)
    {
        UpdateTransform();
    }

    return m_WorldScale;
}

void Transform::SetScale(const Vector3& scale)
{
    if (!m_Parent)
    {
        SetLocalScale(scale);
    }
    else
    {
        SetLocalScale(scale / m_Parent->Scale());
    }
}

Vector3 Transform::Right() const
{
    if (m_HashChanged)
    {
        UpdateTransform();
    }

    return m_WorldRotation * Vector3::Right;
}

Vector3 Transform::Up() const
{
    if (m_HashChanged)
    {
        UpdateTransform();
    }

    return m_WorldRotation * Vector3::Up;
}

Vector3 Transform::Forward() const
{
    if (m_HashChanged)
    {
        UpdateTransform();
    }

    return m_WorldRotation * Vector3::Forward;
}

Vector3 Transform::LocalRight() const
{
    return m_LocalRotation * Vector3::Right;
}

Vector3 Transform::LocalUp() const
{
    return m_LocalRotation * Vector3::Up;
}

Vector3 Transform::LocalForward() const
{
    return m_LocalRotation * Vector3::Forward;
}

Matrix4 Transform::World() const
{
    if (m_HashChanged)
    {
        UpdateTransform();
    }

    return m_World;
}

Matrix4 Transform::WorldToLocalMatrix() const
{
    if (m_HashChanged)
    {
        UpdateTransform();
    }

    return m_InvWorld;
}

Vector3 Transform::LocalPosition() const
{
    return m_LocalPosition;
}

void Transform::SetLocalPosition(const Vector3& position)
{
    HasChanged();
    m_LocalPosition = position;
}

Quaternion Transform::LocalRotation() const
{
    return m_LocalRotation;
}

void Transform::SetLocalRotation(const Quaternion& quat)
{
    HasChanged();
    m_Euler = Quaternion::ToEuler(quat);
    m_LocalRotation = Quaternion::Normalize(quat);
}

void Transform::SetLocalRotation(const Vector3& euler)
{
    HasChanged();
    m_Euler = euler;
    m_LocalRotation = Quaternion::Normalize(Quaternion::Euler(euler));
}

Vector3 Transform::LocalScale() const
{
    return m_LocalScale;
}

void Transform::SetLocalScale(const Vector3& scale)
{
    HasChanged();
    m_LocalScale = scale;
}

void Transform::Translate(const Vector3& translation, Space space)
{
    HasChanged();

    if (space == Space::Self)
    {
        // Moves relative to objects coordinates
        m_LocalPosition += m_LocalRotation * translation;
    }
    else if (space == Space::World)
    {
        if (!m_Parent)
        {
            m_LocalPosition += translation;
        }
        else
        {
            // We need to update the local position, so transform the translation from world space
            // to local space then move the local by that. This is equivlant to moving it in the world
            // frame. But we deffer the world space calculation untill someone requests it.
            m_LocalPosition += Matrix4::Inverse(m_Parent->World()).Transform(translation);
        }
    }
}

// My quat multiplcation seems to be backward to everyone elses???
void Transform::Rotate(const Quaternion& quat, Space space)
{
    HasChanged();

    if (space == Space::Self)
    {
        m_LocalRotation = Quaternion::Normalize(quat * m_LocalRotation);
    }
    else if (space == Space::World)
    {
        if (!m_Parent)
        {
            m_LocalRotation = Quaternion::Normalize(m_LocalRotation * quat);
        }
        else
        {
            Quaternion worldRotation = m_Parent->Rotation();
            m_LocalRotation = Quaternion::Normalize(m_LocalRotation * (Quaternion::Inverse(worldRotation) * quat));
        }
    }
}

void Transform::Rotate(const Vector3& euler, Space space)
{
    Rotate(Quaternion::Euler(euler), space);
}

void Transform::LookAt(const Transform& target, Vector3 worldUp)
{
    Rotate(Quaternion::LookRotation(target.Position(), worldUp), Space::World);
}

void Transform::LookAt(const Vector3& position, Vector3 worldUp)
{
    Rotate(Quaternion::LookRotation(position, worldUp), Space::World);
}

void Transform::HasChanged() const
{
    if (m_HashChanged) { return; }
    m_HashChanged = true;

    // Update all the children to reflect our change!
    for (size_t i = 0; i < m_Children.size(); i++)
    {
        m_Children[i]->HasChanged();
    }
}

void Transform::UpdateTransform() const
{
    if (!m_Parent)
    {
        // Its just the exact same thing
        m_World = Matrix4::Translate(m_LocalPosition) * Matrix4::Rotate(m_LocalRotation) * Matrix4::Scale(m_LocalScale);
        m_WorldPosition = m_LocalPosition;
        m_WorldRotation = m_LocalRotation;
        m_WorldScale = m_LocalScale;
    }
    else
    {
        // Get local relative to parent, then construct matrix.
        m_WorldPosition = m_Parent->World().Transform(m_LocalPosition);
        m_WorldRotation = m_Parent->Rotation() * m_LocalRotation;
        m_WorldScale = m_Parent->Scale() * m_LocalScale;

        m_World = Matrix4::Translate(m_LocalPosition) * Matrix4::Rotate(m_LocalRotation) * Matrix4::Scale(m_LocalScale);
    }

    m_HashChanged = false;
}

void Transform::OnGui()
{
    if (ImGui::CollapsingHeader("Transform: "))
    {
        ImGui::Spacing();
        // Position
        ImGui::Text("Position");
        if (ImGui::DragFloat3("##Position", (float*)&m_LocalPosition, 0.01f, -FLT_MAX, FLT_MAX))
        {
            HasChanged();
        }

        // Rotation
        ImGui::Text("Rotation");
        if (ImGui::DragFloat3("##Rotation", (float*)&m_Euler, 1.0f, -FLT_MAX, FLT_MAX))
        {
            SetLocalRotation(m_Euler);
        }

        // Scale
        ImGui::Checkbox("Uniform Scale: ", &m_UniformScale);
        ImGui::Text("Scale");
        if (m_UniformScale)
        {
            if (ImGui::DragFloat("##Scale", (float*)&m_LocalScale, 1, -FLT_MAX, FLT_MAX))
            {
                m_LocalScale.y = m_LocalScale.x;
                m_LocalScale.z = m_LocalScale.x;
                HasChanged();
            }
        }
        else
        {
            if (ImGui::DragFloat3("##Scale", (float*)&m_LocalScale, 0.1f, -FLT_MAX, FLT_MAX)) { HasChanged(); }
        }
    }
}