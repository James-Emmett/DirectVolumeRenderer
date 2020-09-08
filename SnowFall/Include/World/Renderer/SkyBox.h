//Note:
/*
	SkyBox hold SkyBoxCube, Irradiance, Specular map and BRDF_LUT
	Calling Draw will render skybox as [-1,1] cube in NDC with no depth write.

	Calling Set Textures will bind all the textures too slots [0 - 3]
	Materials textures will start from after Shader global settings etc.

	Maybe we should generate the irradiance and specular convultion ourselves?
*/


#pragma once
#include "Content/Texture.h"
#include "Content/Shader.h"
#include "Math/Matrix4.h"
#include <memory>

class Camera;
class GraphicsDevice;
class ContentManager;
class SkyBox
{
public:
	std::shared_ptr<Texture> m_SkyBox;
	std::shared_ptr<Texture> m_DiffuseEnvHDR;
	std::shared_ptr<Texture> m_SpecularEnvHDR;
	std::shared_ptr<Texture> m_SpecularLUT;
	bool					 m_Enabled = true;

protected:
	GraphicsDevice*			 m_GraphicsDevice = nullptr;
	ContentManager*			 m_ContentManager = nullptr;
	std::shared_ptr<Shader>	 m_Shader;
	BufferHandle			 m_Vertexbuffer;
	BufferHandle			 m_ConstantBuffer;
	Matrix4					 m_WorldVieProj;

public:
	SkyBox();
	virtual ~SkyBox();

public:
	void Initialize(GraphicsDevice* device, ContentManager* contentManager);
	void BindEnviromentMaps();
	virtual void Draw(Matrix4 view, Matrix4 projection);
};