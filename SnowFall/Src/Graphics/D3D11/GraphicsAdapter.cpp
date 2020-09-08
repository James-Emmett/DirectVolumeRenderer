#include "Graphics/GraphicsAdapter.h"
#include "System/StringUtil.h"

#include <dxgi1_6.h>
#include <algorithm>
#include <vector>
#pragma comment(lib, "dxgi.lib")

std::vector<GraphicsAdapter> GraphicsAdapter::g_AdapterList;

bool DisplayMode::operator==(const DisplayMode& mode) const
{
	return	m_Width == mode.m_Width && m_Height == mode.m_Height && m_Numerator == mode.m_Numerator &&
		m_Denominator == mode.m_Denominator && m_Format == mode.m_Format;
}

bool DisplayMode::operator!=(const DisplayMode& mode) const
{
	return !(m_Width == mode.m_Width && m_Height == mode.m_Height && m_Numerator == mode.m_Numerator &&
		m_Denominator == mode.m_Denominator && m_Format == mode.m_Format);
}

bool DisplayMode::operator<(const DisplayMode& mode) const
{
	// Formats match so compare sizes, else just return smallest formats
	if (m_Format == mode.m_Format)
	{
		if (m_Width == mode.m_Width)
		{
			return m_Height < mode.m_Height;
		}
		else
		{
			return m_Width < mode.m_Width;
		}
	}
	else
	{
		return m_Format < mode.m_Format;
	}
}

bool DisplayMode::operator>(const DisplayMode& mode) const
{
	return mode < *this;
}

bool DisplayMode::operator<=(const DisplayMode& mode) const
{
	return !(mode < *this);
}

bool DisplayMode::operator>=(const DisplayMode& mode) const
{
	return !(*this < mode);
}

std::string GraphicsAdapter::DeviceName() const
{
	return m_DeviceName;
}

GpuVender GraphicsAdapter::Vender() const
{
	return m_Vender;
}

Uint64 GraphicsAdapter::Ram() const
{
	return m_Ram;
}

Uint64 GraphicsAdapter::VRam() const
{
	return m_VRam;
}

AdapterID GraphicsAdapter::ID() const
{
	return m_AdapterID;
}

DisplayMode GraphicsAdapter::GetDefaultMode() const
{
	if (m_DisplayModes.empty())
	{
		return DisplayMode();
	}
	return m_DisplayModes.front();
}

const std::vector<DisplayMode>& GraphicsAdapter::DisplayModes() const
{
	return m_DisplayModes;
}

bool GraphicsAdapter::IsModeValid(const DisplayMode& mode) const
{
	return std::find(m_DisplayModes.begin(), m_DisplayModes.end(), mode) != m_DisplayModes.end();
}

const GraphicsAdapter* GraphicsAdapter::GetAdapterFromLUID(AdapterID& id)
{
	// Call get incase its not initialized already.
	const std::vector<GraphicsAdapter>* graphicsAdapters = GetAdapterList();
	for (size_t i = 0; i < graphicsAdapters->size(); ++i)
	{
		const GraphicsAdapter* adapter = &graphicsAdapters->at(i);
		if (adapter->m_AdapterID == id)
		{
			return adapter;
		}
	}

	return nullptr;
}

const GraphicsAdapter* GraphicsAdapter::GetDefaultAdapter()
{
	const std::vector<GraphicsAdapter>* graphicsAdapters = GetAdapterList();
	return &graphicsAdapters->front();
}

const std::vector<GraphicsAdapter>* GraphicsAdapter::GetAdapterList()
{
	if (g_AdapterList.empty())
	{
		IDXGIFactory6* factory = nullptr;
		IDXGIAdapter* adapter = nullptr;
		IDXGIOutput* output = nullptr;
		DXGI_ADAPTER_DESC adapterDesc = {};
		DXGI_OUTDUPL_DESC outputDesc = {};

		HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
		if (SUCCEEDED(hr))
		{
			// Loop through each adapter in factory
			for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				GraphicsAdapter graphicsAdapter;
				if (FAILED(adapter->GetDesc(&adapterDesc)))
				{
					// Skip adapter something is wrong
					continue;
				}

				// Extra info
				graphicsAdapter.m_Vender = (GpuVender)adapterDesc.VendorId;
				graphicsAdapter.m_DeviceName = StringUtil::Narrow(adapterDesc.Description);
				graphicsAdapter.m_AdapterID.HighPart = adapterDesc.AdapterLuid.HighPart; // For HMD!
				graphicsAdapter.m_AdapterID.LowPart = adapterDesc.AdapterLuid.LowPart;  // For HMD!
				graphicsAdapter.m_Ram = (Uint64)adapterDesc.DedicatedSystemMemory;
				graphicsAdapter.m_VRam = (Uint64)adapterDesc.DedicatedVideoMemory;

				for (UINT i = 0; adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND; ++i)
				{
					IDXGIOutput1* output1 = nullptr;
					if (SUCCEEDED(output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1)))
					{
						UINT numModes = 0;
						if (SUCCEEDED(output1->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_STEREO, &numModes, NULL)))
						{
							DXGI_MODE_DESC1* displayModes = new DXGI_MODE_DESC1[numModes];

							if (SUCCEEDED(output1->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_STEREO, &numModes, displayModes)))
							{
								for (size_t i = 0; i < numModes; i++)
								{
									DisplayMode mode;

									// Warning about casign to 32bit, will a resolution ever be 4,294,967,295 width or in height? 
									mode.m_Width = (Uint32)displayModes[i].Width;
									mode.m_Height = (Uint32)displayModes[i].Height;
									mode.m_Format = (SurfaceFormat)displayModes[i].Format;
									mode.m_Numerator = (Uint32)displayModes[i].RefreshRate.Numerator;
									mode.m_Denominator = (Uint32)displayModes[i].RefreshRate.Denominator;
									mode.m_Stero = displayModes[i].Stereo;

									graphicsAdapter.m_DisplayModes.push_back(mode);
								}
							}

							std::sort(graphicsAdapter.m_DisplayModes.begin(), graphicsAdapter.m_DisplayModes.end(), std::greater<>());
							output1->Release();
							delete[] displayModes;
						}
					}
					g_AdapterList.push_back(graphicsAdapter);
				}

				adapter->Release();
			}

			factory->Release();
		}
	}
	return &g_AdapterList;
}