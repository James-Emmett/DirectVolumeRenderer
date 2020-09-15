#include "World/Renderer/PostProcess/PostProcessor.h"
#include "Content/Shader.h"

PostProcessor::PostProcessor()
{
}

PostProcessor::~PostProcessor()
{
	Release();
}

void PostProcessor::Initialize(GraphicsDevice* device, int executionOrder)
{
	m_GraphicsDevice = device;
	m_Order = executionOrder;
}

void PostProcessor::Release()
{
	m_GraphicsDevice = nullptr;
	m_Effect.reset();
}

void PostProcessor::Apply(RenderHandle source, RenderHandle destination)
{
	DrawFullScreenQuad(source, destination, m_Effect);
}

void PostProcessor::OnGui()
{
}

bool PostProcessor::operator>(const PostProcessor& pass)
{
	return m_Order > pass.m_Order;
}

bool PostProcessor::operator<(const PostProcessor& pass)
{
	return m_Order < pass.m_Order;
}

bool PostProcessor::operator>=(const PostProcessor& pass)
{
	return m_Order >= pass.m_Order;
}

bool PostProcessor::operator<=(const PostProcessor& pass)
{
	return m_Order <= pass.m_Order;;
}

void PostProcessor::DrawFullScreenQuad(TextureHandle texture, RenderHandle target, std::shared_ptr<Shader> shader)
{
	assert(m_Effect);
	m_GraphicsDevice->BlitToBuffer(texture, target, shader->GetPipeline());
}