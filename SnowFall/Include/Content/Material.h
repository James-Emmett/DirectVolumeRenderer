//Note:
/*
	Need a custom vector class!!!
	Linear O(n) better than multiple STL maps for cache and low
	numbers. Aras came to this conclusion as well: https://aras-p.info/blog/2015/04/04/optimizing-unity-renderer-2-cleanups/

	Opted for poor memory block as opposed to previous byte order offset thing
*/
#pragma once
#include "Graphics/GraphicsDevice.h"
#include "Texture.h"
#include "Shader.h"
#include "Resource.h"
#include "Math/Vector4.h"
#include <memory>
#include <World\Renderer\RenderCommon.h>

class Material : public Resource
{
protected:
	GraphicsDevice*							m_GraphicsDevice = nullptr;
	std::vector<std::shared_ptr<Texture>>	m_Textures;	// Use Resource Manager!
	std::vector<Byte>						m_Buffer;	// Padded for 16 byte alignment.
	std::shared_ptr<Shader>					m_Shader;	// Shader this material depends on!
	std::vector<Uint32>						m_Offsets;	// Offsets for various properties
	BufferHandle							m_ConstantBuffer;
	bool									m_Dirty = false;

public:
	Material();
	Material(std::shared_ptr<Shader> shader);
	Material(const Material& mat) = delete;
	Material(Material&& mat) noexcept;
	~Material();

public:
	void operator=(const Material& mat) = delete;
	void operator=(Material&& mat) noexcept;

public:
	float GetFloat(std::string name)const;
	float GetFloat(int id)const;
	void SetFloat(std::string name, float value);
	void SetFloat(int id, float value);

	int GetInt(std::string name)const;
	int GetInt(int id)const;
	void SetInt(std::string name, int value);
	void SetInt(int id, int value);

	Vector2 GetVector2(std::string name)const;
	Vector2 GetVector2(int id)const;
	void SetVector2(std::string name, Vector2 value);
	void SetVector2(int id, Vector2 value);

	Vector3 GetVector3(std::string name)const;
	Vector3 GetVector3(int id)const;
	void SetVector3(std::string name, Vector3 value);
	void SetVector3(int id, Vector3 value);

	Vector4 GetVector4(std::string name)const;
	Vector4 GetVector4(int id)const;
	void SetVector4(std::string name, Vector4 value);
	void SetVector4(int id, Vector4 value);

	Color GetColor(std::string name)const;
	Color GetColor(int id)const;
	void SetColor(std::string name, Color value);
	void SetColor(int id, Color value);

	const std::shared_ptr<Texture> GetTexture(std::string name)const;
	const std::shared_ptr<Texture> GetTexture(int id)const;
	void SetTexture(std::string name, std::shared_ptr<Texture> value);
	void SetTexture(int id, std::shared_ptr<Texture> value);
	bool HasProperty(std::string name);

	const std::vector<std::shared_ptr<Texture>>& GetTextureArray()const;
	std::shared_ptr<Shader> GetShader() const;
	BufferHandle GetConstantBuffer()const;
	void LoadFromFile(const std::string& filePath);
	void Reload();
	void OnGui();
	void Bind(Uint32 startTextureSlot = (Uint32)TextureDefaults::Count, Uint32 startBuffer = (Uint32)UniformTypes::Count);
	bool SaveToFile(std::string fileName); // Editor only!
	void Release();

private:
	bool LoadFromBinary(const std::string& filePath);
	bool LoadFromXML(const std::string& filePath);
	void AddTextureProperty(const char* name, const char* source);
	void AddValueProperty(const char* name, PropertyType type, float X, float Y = 0, float Z = 0, float W = 0);
	Uint32 ResizeAlignBuffer(Uint32 byteCount);

};