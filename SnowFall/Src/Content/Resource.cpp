#include "Content/Resource.h"
//#include "Imgui/imgui.h"

Resource::Resource()
{
}

// Move Constructor
Resource::Resource(Resource&& resource) noexcept
{
	m_Name		= resource.m_Name;
	m_FilePath	= resource.m_FilePath;
	m_Type		= resource.m_Type;
	m_LoadState = resource.m_LoadState;
}

std::string Resource::ResourcePath() const
{
	return m_FilePath;
}

void Resource::SetResourcePath(std::string path)
{
	m_FilePath = path;
}

bool Resource::IsLoaded() const
{
	return m_LoadState == LoadState::Loaded;
}

LoadState Resource::LoadState() const
{
	return m_LoadState;
}

Uint32 Resource::Size() const
{
	return m_Size;
}

void Resource::OnGui()
{
}