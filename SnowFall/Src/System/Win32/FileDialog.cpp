//NOTE:
/*
	Need to add filters using IShellItemFilter somehow.
*/

#include "System/FileDialog.h"
#include "System/Logger.h"
#include "System/StringUtil.h"
#include "System/Assert.h"
#include <windows.h>
#include <ShObjIdl.h>
#include <vector>

HRESULT SetDefaultPath(IFileDialog* fileDialog, const std::string& defaultPath)
{
	// Skip the file select.s
	if (defaultPath.empty()) { return S_OK; }

	IShellItem* folder;
	std::wstring pDefaultPathW = StringUtil::Widen(defaultPath);
	HRESULT result = SHCreateItemFromParsingName(pDefaultPathW.c_str(), NULL, IID_PPV_ARGS(&folder));

	if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || result == HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE))
	{
		LogError("Default Path not Found");
		assert(0);
	}
	else if (result == S_OK)
	{
		fileDialog->SetFolder(folder);
		folder->Release();
	}

	return result;
}

HRESULT SetFilters(IFileDialog* fileDialog, const std::string& filters)
{
	// No filters return.
	if (filters.empty()) { return S_OK; }

	// Split the different types using :
	std::vector<std::string> filterTypeList = StringUtil::Split(filters, ":");
	size_t filterCount = filterTypeList.size();

	HRESULT hr = S_OK;
	COMDLG_FILTERSPEC* filterSpecs = new COMDLG_FILTERSPEC[filterCount];

	// Set the easy name and filters seperated by |
	for (size_t i = 0; i < filterCount; i++)
	{
		std::vector<std::string> spec = StringUtil::Split(filterTypeList[i], "|");
		if (spec.size() == 2)
		{
			filterSpecs[i].pszName = StringUtil::WidenToPointer(spec[0]);
			filterSpecs[i].pszSpec = StringUtil::WidenToPointer(spec[1]);
		}
	}

	hr = fileDialog->SetFileTypes((UINT)filterCount, filterSpecs);

	// release pointers
	for (size_t i = 0; i < filterCount; ++i)
	{
		delete filterSpecs[i].pszName;
		delete filterSpecs[i].pszSpec;
	}
	delete[] filterSpecs;
	return hr;
}

DialogResult FileDialog::OpenDialog(const std::string filters, const std::string defaultPath, std::string& outPath)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Initialize thread for COM
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen = nullptr;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			hr = SetFilters(pFileOpen, filters);
			if (SUCCEEDED(hr))
			{
				// Set the default path
				hr = SetDefaultPath(pFileOpen, defaultPath);
				if (SUCCEEDED(hr))
				{
					// Show Dialog
					hr = pFileOpen->Show(NULL);
					if (SUCCEEDED(hr))
					{
						// Get file name from dialog
						IShellItem* pItem;
						hr = pFileOpen->GetResult(&pItem);

						if (SUCCEEDED(hr))
						{
							wchar_t* filePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
							if (SUCCEEDED(hr))
							{
								outPath = StringUtil::Narrow(filePath);
								CoTaskMemFree(filePath);
							}
							pItem->Release();
						}
					}
					pFileOpen->Release();
				}
			}
		}
		CoUninitialize();
	}

	if (hr == S_OK)
	{
		return DialogResult::Ok;
	}

	if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		return DialogResult::Cancel;
	}

	return DialogResult::Error;
}

DialogResult FileDialog::SaveDialog(const std::string filters, const std::string defaultPath, std::string& outPath)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Initialize thread for COM
	if (SUCCEEDED(hr))
	{
		IFileSaveDialog* pFileSave = nullptr;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(hr))
		{
			hr = SetFilters(pFileSave, filters);
			if (SUCCEEDED(hr))
			{
				// Set the default path
				hr = SetDefaultPath(pFileSave, defaultPath);
				if (SUCCEEDED(hr))
				{
					// Show Dialog
					hr = pFileSave->Show(NULL);
					if (SUCCEEDED(hr))
					{
						// Get file name from dialog
						IShellItem* pItem;
						hr = pFileSave->GetResult(&pItem);

						if (SUCCEEDED(hr))
						{
							wchar_t* filePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
							if (SUCCEEDED(hr))
							{
								outPath = StringUtil::Narrow(filePath);
								CoTaskMemFree(filePath);
							}
							pItem->Release();
						}
					}
					pFileSave->Release();
				}
			}
		}
		CoUninitialize();
	}

	if (hr == S_OK)
	{
		return DialogResult::Ok;
	}

	if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		return DialogResult::Cancel;
	}

	return DialogResult::Error;
}

DialogResult FileDialog::OpenFolderDialog(const std::string defaultPath, std::string& outPath)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	// Initialize thread for COM
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen = nullptr;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Set the default path
			hr = SetDefaultPath(pFileOpen, defaultPath);
			if (SUCCEEDED(hr))
			{
				FILEOPENDIALOGOPTIONS options;
				if (SUCCEEDED(pFileOpen->GetOptions(&options)) && SUCCEEDED(pFileOpen->SetOptions(options | FOS_PICKFOLDERS)))
				{

					// Show Dialog
					hr = pFileOpen->Show(NULL);
					if (SUCCEEDED(hr))
					{
						// Get file name from dialog
						IShellItem* pItem;
						hr = pFileOpen->GetResult(&pItem);

						if (SUCCEEDED(hr))
						{
							wchar_t* filePath;
							hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
							if (SUCCEEDED(hr))
							{
								outPath = StringUtil::Narrow(filePath);
								CoTaskMemFree(filePath);
							}
							pItem->Release();
						}
					}
					pFileOpen->Release();
				}
			}
		}
		CoUninitialize();
	}

	if (hr == S_OK)
	{
		return DialogResult::Ok;
	}

	if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
	{
		return DialogResult::Cancel;
	}

	return DialogResult::Error;
}