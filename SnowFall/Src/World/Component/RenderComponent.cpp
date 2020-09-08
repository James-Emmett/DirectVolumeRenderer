#include "World/Component/RenderComponent.h"

RenderType RenderComponent::GetRenderQueue() const
{
    if (m_Material)
    {
        return m_Material->GetShader()->GetRenderQueue();
    }

    return RenderType::Opaque;
}
