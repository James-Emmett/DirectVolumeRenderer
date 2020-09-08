#pragma once
#include <string>

enum class DialogResult
{
	Error,
	Ok,
	Cancel,
};

namespace FileDialog
{
	DialogResult OpenDialog(const std::string filters, const std::string defaultPath, std::string& outPath);
	DialogResult SaveDialog(const std::string filters, const std::string defaultPath, std::string& outPath);
	DialogResult OpenFolderDialog(const std::string defaultPath, std::string& outPath);
};