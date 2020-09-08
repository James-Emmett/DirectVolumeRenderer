#pragma once
#include "System/StringUtil.h"

namespace StringUtil
{

	std::string Narrow(const wchar_t* s)
	{
		size_t wideLength = wcslen(s);
		size_t requiredSize = 0;

		if (wideLength != 0)
		{
			wcstombs_s(&requiredSize, nullptr, 0, s, wideLength);

			if (requiredSize != 0)
			{
				char* buffer = new char[requiredSize + 1];

				if (wcstombs_s(&requiredSize, buffer, requiredSize, s, wideLength) == 0)
				{
					std::string string(buffer);
					delete[] buffer;
					return string;
				}
			}
		}

		return std::string();
	}

	std::wstring Widen(const char* s)
	{
		size_t strLength = strlen(s);
		size_t requiredSize = 0;

		if (strLength != 0)
		{
			mbstowcs_s(&requiredSize, nullptr, 0, s, strLength);

			if (requiredSize != 0)
			{
				// Null terminate is size of a wchar_t 2 - 4 bytes D.O.P
				wchar_t* buffer = new wchar_t[requiredSize + sizeof(wchar_t)];

				if (mbstowcs_s(&requiredSize, buffer, requiredSize, s, strLength) == 0)
				{
					std::wstring string(buffer);
					delete[] buffer;
					return string;
				}
			}
		}

		return std::wstring();
	}

	std::string Narrow(const std::wstring& s)
	{
		size_t wideLength = s.length();
		size_t requiredSize = 0;

		if (wideLength != 0)
		{
			wcstombs_s(&requiredSize, nullptr, 0, s.c_str(), wideLength);

			if (requiredSize != 0)
			{
				char* buffer = new char[requiredSize + 1];

				if (wcstombs_s(&requiredSize, buffer, requiredSize, s.c_str(), wideLength) == 0)
				{
					std::string string(buffer);
					delete[] buffer;
					return string;
				}
			}
		}

		return std::string();
	}

	std::wstring Widen(const std::string& s)
	{
		size_t strLength = s.length();
		size_t requiredSize = 0;

		if (strLength != 0)
		{
			mbstowcs_s(&requiredSize, nullptr, 0, s.c_str(), strLength);

			if (requiredSize != 0)
			{
				// Null terminate is size of a wchar_t 2- 4 bytes D.O.P
				wchar_t* buffer = new wchar_t[requiredSize + sizeof(wchar_t)];

				if (mbstowcs_s(&requiredSize, buffer, requiredSize, s.c_str(), strLength) == 0)
				{
					std::wstring string(buffer);
					delete[] buffer;
					return string;
				}
			}
		}

		return std::wstring();
	}

	wchar_t* WidenToPointer(const std::string& s)
	{
		size_t strLength = s.length();
		size_t requiredSize = 0;

		if (strLength != 0)
		{
			mbstowcs_s(&requiredSize, nullptr, 0, s.c_str(), strLength);

			if (requiredSize != 0)
			{
				// Null terminate is size of a wchar_t 2- 4 bytes D.O.P
				wchar_t* buffer = new wchar_t[requiredSize + sizeof(wchar_t)];

				if (mbstowcs_s(&requiredSize, buffer, requiredSize, s.c_str(), strLength) == 0)
				{
					return buffer;
				}
			}
		}

		return nullptr;
	}

	std::vector<std::string> Split(const std::string& text, const char* deliminater)
	{
		std::vector<std::string> stringList;

		int start = 0;
		int end = (int)text.find_first_of(deliminater);

		while (end != std::string::npos)
		{
			// Should be using move constructor so not as ineffecient?
			std::string string = text.substr(start, end - start);
			if (!string.empty())
			{
				stringList.push_back(string);
			}
			start = end + 1;
			end = (int)text.find_first_of(deliminater, start);
		}

		// Push rest of buffer to end!
		stringList.push_back(text.substr(start));

		return stringList;
	}
};