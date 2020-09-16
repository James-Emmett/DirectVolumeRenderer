//NOTE:
/*
	Tone Mapping is a special case final stage post processor that blits to backbuffer?
*/

#pragma once
#include <memory>
#include "Graphics/Graphics.h"

struct ToneMappingSettings
{
	float _Gamma = 2.2f;
	float _Exposure = 1.8f;
	int	  _Tonetype = 1;
	float _Pad_01 = 0.0f;
};

class GraphicsDevice;
class Shader;
class ToneMapping
{
private:
	GraphicsDevice* m_GraphicsDevice = nullptr;
	ToneMappingSettings m_ToneSettings;
	BufferHandle m_ToneConstBuffer;
	std::shared_ptr<Shader> m_Effect;

public: 
	void Initialize(GraphicsDevice* device);
	void Release();
	void Apply(RenderHandle source);
	void OnGui();
};