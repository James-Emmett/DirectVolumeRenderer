//Note:
/*
	Generic resoruce loader, did have unique but at this point this will do.
*/


#pragma once
#include "Resource.h"
#include "System/Assert.h"
#include <string>
#include <unordered_map>
#include <memory>

class ContentManager
{
private:
	typedef std::unordered_map<std::string, std::shared_ptr<Resource>> ResourceMap;
	ResourceMap m_Resources;

public:
	template<typename T>
	std::shared_ptr<T> Load(std::string path)
	{
		assert(path.empty() == false);
		std::string filePath = path;

		typename std::unordered_map<std::string, std::shared_ptr<Resource>>::iterator itr = m_Resources.find(filePath);
		if (itr != m_Resources.end())
		{
			return std::dynamic_pointer_cast<T>(itr->second);
		}

		std::shared_ptr<T> resource = std::make_shared<T>();
		resource->LoadFromFile(filePath);
		m_Resources.emplace(filePath, std::dynamic_pointer_cast<Resource>(resource));
		return resource;
	}

	void Unload(std::string path)
	{
		m_Resources.erase(path);
	}

	void Clear()
	{
		m_Resources.clear();
	}
};