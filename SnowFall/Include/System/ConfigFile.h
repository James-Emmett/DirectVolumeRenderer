//Note:
/*
	Common implementation, in this case its
	a modified ogre style system. Seen it in unity a few times though...
*/

#pragma once
#include "File.h"
#include <map>

typedef std::map<std::string, std::string> SettingsMap;
typedef std::map<std::string, SettingsMap> Section;

// Setting iterator
typedef std::map<std::string, std::string>::iterator SettingsIterator;
typedef std::map<std::string, std::string>::const_iterator SettingsIteratorConst;

// Section Iterator
typedef std::map<std::string, SettingsMap>::iterator SectionIterator;
typedef std::map<std::string, SettingsMap>::const_iterator SectionIteratorConst;

class ConfigFile
{
private:
	Section m_Settings;
	std::string m_FileName = "";
	bool m_Open = false;

public:
	ConfigFile();
	~ConfigFile();

public:
	bool Open(std::string fileName);
	void Close();
	bool IsOpen()const;
	void Save()const;
	void Clear();

	const std::string& GetString(const std::string& section, const std::string& key, std::string defaultValue = "");
	void SetString(const std::string& section, const std::string& key, const std::string& value);
	int  GetInt(const std::string& section, const std::string& key, int defaultValue = 0);
	void SetInt(const std::string& section, const std::string& key, int value);
	float GetFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f);
	void SetFloat(const std::string& section, const std::string& key, float value);
	bool GetBool(const std::string& section, const std::string& key, bool defaultValue = false);
	void SetBool(const std::string& section, const std::string& key, bool value);
};