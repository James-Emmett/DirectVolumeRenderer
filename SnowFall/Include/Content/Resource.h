//Note:
/*
    Base resource class for all resources (texture, mesh, user defined...)
    Contains basic info such as filepath, magic value, size and ref count for automatic dissposal.
*/

#pragma once
#include "System/Types.h"
#include <string>

enum class LoadState
{
    Unloaded,
    Unloading,
    Loading,
    Loaded,
    Failed,
};

class Resource
{
protected:
    std::string m_Name = "Mesh";
    std::string	m_FilePath = "";
    Uint32	    m_Type = 0;    // Magic
    LoadState	m_LoadState = LoadState::Unloaded;
    Uint32		m_Size = 0;	 // Size in bytes of resource

public:
    Resource();
    virtual ~Resource() { }
    Resource(Resource&& resource)noexcept; // Move Constructor

public:
    std::string	ResourcePath()const;
    void        SetResourcePath(std::string path);
    bool		IsLoaded()const;
    LoadState	LoadState()const;
    Uint32		Size()const;

    virtual void LoadFromFile(const std::string& filePath) = 0;
    virtual bool SaveToFile(std::string fileName) = 0; // Editor only?
    virtual void Reload() = 0;
    virtual void OnGui();
    virtual void Release() = 0;
};