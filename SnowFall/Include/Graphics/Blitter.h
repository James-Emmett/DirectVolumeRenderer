#include "GraphicsDevice.h"
#include "Content/Shader.h"
#include "Content/ContentManager.h"
#include <memory>

class Blitter
{
	GraphicsDevice* m_GraphicsDevice;
	std::shared_ptr<Shader> m_BlitShader;

public:
	void Initialize(GraphicsDevice* device, ContentManager* contentManager)
	{
		m_GraphicsDevice = device;
		m_BlitShader = contentManager->Load<Shader>("Assets/Shaders/PostProcess/blit.shader");
	}

	void BlitToBuffer(TextureHandle source, RenderHandle target)
	{
		m_BlitShader->Bind();
		m_GraphicsDevice->ClearRenderTarget(target, Color::Black);
		m_GraphicsDevice->BindRenderTarget(target);
		m_GraphicsDevice->BindTexture(source, 0);
		m_GraphicsDevice->Draw(3, 0, 0);
	}

	// blit the source into what ever target is currently bound
	void BlitToBound(TextureHandle source)
	{
		m_BlitShader->Bind();
		m_GraphicsDevice->BindTexture(source, 0);
		m_GraphicsDevice->Draw(3, 0, 0);
	}
};