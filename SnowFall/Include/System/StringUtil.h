#pragma once
#include <string>
#include <vector>

namespace StringUtil
{
	std::string Narrow(const wchar_t* s);
	std::wstring Widen(const char* s);
	std::string Narrow(const std::wstring& s);
	std::wstring Widen(const std::string& s);
	wchar_t* WidenToPointer(const std::string& s);

	// Creates buffer on heap, your responsible for deleting it!
	std::vector<std::string> Split(const std::string& text, const char* deliminater);
};