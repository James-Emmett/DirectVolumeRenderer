#pragma once
#include "Math/Vector4.h"
#include "Math/Mathf.h"
#include "Content/Texture.h"
#include "Application/Application.h"
#include "System/File.h"
#include "UI/ImGui_Interface.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>

struct TransferNode
{
	float R = 0.0f;
	float G = 0.0f;
	float B = 0.0f;
	float Roughness = 1.0f;
	float Metallic = 0.0f;
	float Opacity = 0.0f;
	Uint32 Intensity = 0;

	TransferNode()
	{
		R = 0.0f; G = 0.0f; B = 0.0f;
		Roughness = 1.0f;
		Metallic = 0.0f;
		Opacity = 1.0f;
		Intensity = 0;
	}

	TransferNode(Uint32 intensity, float opacity, float r, float g, float b, float roughness, float metallic)
	{
		R = r; G = g; B = b;
		Roughness = roughness;
		Metallic = metallic;
		Opacity = opacity;
		Intensity = intensity;
	}

	TransferNode(Uint32 intensity, float opacity)
	{
		Opacity = opacity;
		Intensity = intensity;
	}

	Uint32 GetIntensity()const
	{
		return Intensity;
	}

	void SetIntensity(Uint32 intensity)
	{
		Intensity = intensity;
	}

	void SetOpacity(float opacity)
	{
		Opacity = opacity;
	}

	float GetOpacity()const
	{
		return Opacity;
	}

	// Return interpolated node
	static TransferNode Lerp(TransferNode& n1, TransferNode& n2, float t)
	{
		float minusT = (1.0f - t);
		TransferNode node;
		node.R = minusT * n1.R + (n2.R * t);
		node.G = minusT * n1.G + (n2.G * t);
		node.B = minusT * n1.B + (n2.B * t);
		node.Roughness = minusT * n1.Roughness + (n2.Roughness * t);
		node.Metallic = minusT * n1.Metallic + (n2.Metallic * t);
		node.Intensity = (Uint32)(minusT * n1.Intensity + (n2.Intensity * t));
		node.Opacity = minusT * n1.Opacity + (n2.Opacity * t);
		return node;
	}

	static bool Sorter(const TransferNode& lhs, const TransferNode& rhs)
	{
		return lhs.Intensity < rhs.Intensity;
	}
};

// Add cubic if get time, this is a bad approximation
// But Exposure render does the same thing.
// Dont hardcode strings, change when works!
class TransferFunction
{
private:
	// Reduce to a single Texture2D with point sampling?
	std::shared_ptr<Texture>  m_TransferTex;
	std::vector<TransferNode> m_Nodes;
	std::string m_FilePath;
	int m_DraggingNode = -1;
	int m_SelectedNode = -1;
	bool m_CanvasClicked = false;
	bool m_Dirty = false;
	bool m_MouseDownPrevious = false;

public:
	void Initialize(std::string path)
	{
		m_Nodes.clear();
		Load(path);
	}

	void ShutDown()
	{
		SaveTransfer(m_FilePath);
	}

	void Load(std::string filePath)
	{
		m_FilePath = filePath;

		// Load saved data, else create
		BinaryFile file;
		if (file.Open(m_FilePath.c_str(), FileMode::Read))
		{
			int nodeCount = file.ReadDword();
			m_Nodes.resize(nodeCount);
			file.ReadBuffer((Byte*)&m_Nodes[0], nodeCount * sizeof(TransferNode));
			file.Close();
		}
		else
		{
			// atleast 2 nodes black to white
			m_Nodes.push_back(TransferNode(0, 0.0f, 1, 0, 0, 0, 0));
			m_Nodes.push_back(TransferNode(64, 0.25f, 0, 1, 0, 0.5f, 0));
			m_Nodes.push_back(TransferNode(128, 0.75f, 1, 0, 1, 0.5f, 0));
			m_Nodes.push_back(TransferNode(255, 1.0f, 0, 0, 1, 0.5f, 0));
		}
		if (m_TransferTex == nullptr)
		{
			m_TransferTex = std::make_shared<Texture>();
			m_TransferTex->Create2D(255, 2, 1, false, BufferUsage::Dynamic, SurfaceFormat::R8G8B8A8_Unorm);
			m_TransferTex->SetFilter(FilterMode::MinMagMipPoint);
		}

		// Do an initial generation pass?
		GenerateTransferFunction();
	}

	void SaveTransfer(std::string filePath = "")
	{
		if (!filePath.empty())
		{
			m_FilePath = filePath;
		}

		if (m_FilePath.empty()) { return; }

		BinaryFile file;
		if (file.Open(m_FilePath.c_str(), FileMode::Write))
		{
			int nodeCount = (int)m_Nodes.size();
			file.WriteDword(nodeCount);
			file.WriteBuffer((Byte*)&m_Nodes[0], nodeCount * sizeof(TransferNode));
			file.Close();
		}
	}

	std::shared_ptr<Texture> GetDiffuseTransfer()
	{
		return m_TransferTex;
	}

	void AddNode(TransferNode node)
	{
		m_Nodes.push_back(node);
	}

	Uint32 SegmentCount()const
	{
		return ((Uint32)m_Nodes.size() - 1);
	}

	// Computes the textures for the transfer
	void GenerateTransferFunction()
	{
		Byte* colorData = m_TransferTex->GetData();
		Uint32 pitch = m_TransferTex->GetRowPitch();

		// Go throguh each node, accept last one as theres no lerp for it!
		int pixel = 0;
		for (Uint32 i = 0; i < m_Nodes.size() - 1; i++)
		{
			// How many iso values between these nodes, for t value!
			Uint32 steps = (Uint32)(m_Nodes[i + 1].GetIntensity() - m_Nodes[i].GetIntensity());

			for (Uint32 j = 0; j < steps; j++)
			{
				int colorIndex = pixel * 4;
				int metalIndex = pitch + (pixel * 4);

				float t = (float)j / (float)(steps - 1);
				TransferNode node = TransferNode::Lerp(m_Nodes[i], m_Nodes[i + 1], t);

				colorData[colorIndex] = (Byte)(node.R * 255);
				colorData[colorIndex + 1] = (Byte)(node.G * 255);
				colorData[colorIndex + 2] = (Byte)(node.B * 255);
				colorData[colorIndex + 3] = (Byte)(node.GetOpacity() * 255);

				colorData[metalIndex] = (Byte)(node.Metallic * 255);
				colorData[metalIndex + 1] = (Byte)(node.Roughness * 255);
				colorData[metalIndex + 2] = 0;
				colorData[metalIndex + 3] = 255;
				pixel++;
			}
		}

		// Update the new data to the GPU
		m_TransferTex->Apply(true);
		m_Dirty = false;
	}

	// Returns true if edited in any way and its finsihed editign too
	bool DisplayEditor()
	{
		// Reference for how i figured out imgui point drawing:
		// https://github.com/Twinklebear/imgui-transfer-function/blob/master/transfer_function_widget.cpp

		if (ImGui::CollapsingHeader("Transfer Function"))
		{
			ImGui::Separator();
			ImGui::Image(m_TransferTex->GetTextureResource()->m_SRV, ImVec2(ImGui::GetContentRegionAvail().x, 16), ImVec2(0, 0.0f), ImVec2(1, 0.5f));
			ImGui::Image(m_TransferTex->GetTextureResource()->m_SRV, ImVec2(ImGui::GetContentRegionAvail().x, 16), ImVec2(0, 0.5f), ImVec2(1, 1));

			ImVec2 canvasSize = ImGui::GetContentRegionAvail(); // avliable space
			canvasSize.y = canvasSize.y * 0.8f; // leave 20% room for button GUI
			ImVec2 canvasPos = ImGui::GetCursorScreenPos(); // Position imgui is currently at

			// Drawlist to store draw commands
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			// Rectangle to clip custom draw list content
			drawList->PushClipRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y));

			// View scale stores width of the rect to draw inside, the y is negated
			// as we need to push dots up from the imgui cursor pos i.e apha of 1 = -canvasSize.y
			Vector2 viewScale = Vector2(canvasSize.x, -canvasSize.y);
			// View offset is the bottom of the draw rect, we minus canvase.y for the nodes!
			Vector2 viewOffset(canvasPos.x, canvasPos.y + canvasSize.y);

			// Border Rectangle
			drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y), ImColor(180, 180, 180, 255));

			// Invisible button to encompass transfer graph to handle custom picking/selection.
			ImGui::InvisibleButton("Transfer_Canvas", canvasSize);

			// Check for mouse click
			ImGuiIO io = ImGui::GetIO();
			if (!io.MouseDown[0] && !io.MouseDown[1])
			{
				m_CanvasClicked = false;
			}
			if (ImGui::IsItemHovered() && (io.MouseDown[0] || io.MouseDown[1]))
			{
				m_CanvasClicked = true;
			}

			// Clamp mouse position within the rectangle
			Vector2 bbmin = Vector2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y);
			Vector2 bbmax = Vector2(ImGui::GetItemRectMax().x, ImGui::GetItemRectMax().y);
			Vector2 clippedMousePos = Vector2(Mathf::Min(Mathf::Max(io.MousePos.x, bbmin.x), bbmax.x),
				Mathf::Min(Mathf::Max(io.MousePos.y, bbmin.y), bbmax.y));

			if (m_CanvasClicked)
			{
				// Normalize the mouse within the rext 
				Vector2 mousePos = (Vector2((clippedMousePos.x - viewOffset.x) / viewScale.x,
					(clippedMousePos.y - viewOffset.y) / viewScale.y));
				mousePos.x = Mathf::Clamp(mousePos.x, 0.f, 1.f);
				mousePos.y = Mathf::Clamp(mousePos.y, 0.f, 1.f);

				if (io.MouseDown[0] == true)
				{
					if (m_DraggingNode != -1)
					{
						// Only move if moved more than x amount?
						if (Vector3::DistanceSquared(mousePos, m_Nodes[m_DraggingNode].Intensity * 0.00392156862f) > 0.025f * 0.025f)
						{
							// Update node too mouse location
							m_Nodes[m_DraggingNode].SetIntensity((Uint8)(mousePos.x * 255));
							m_Nodes[m_DraggingNode].SetOpacity(mousePos.y);

							// clamp first and last x
							if (m_DraggingNode == 0)
							{
								m_Nodes[0].Intensity = 0;
							}
							else if (m_DraggingNode == m_Nodes.size() - 1)
							{
								m_Nodes[m_DraggingNode].Intensity = 255;
							}
							else
							{
								// Clamp between nodes
								if (m_Nodes[m_DraggingNode].Intensity <= m_Nodes[m_DraggingNode - 1].Intensity)
								{
									m_Nodes[m_DraggingNode].Intensity = m_Nodes[m_DraggingNode - 1].Intensity + 5;
								}
								else if (m_Nodes[m_DraggingNode].Intensity >= m_Nodes[m_DraggingNode + 1].Intensity)
								{
									m_Nodes[m_DraggingNode].Intensity = m_Nodes[m_DraggingNode + 1].Intensity - 5;
								}
							}

							m_Dirty = true; // Should this be here?
						}
					}
					else if (m_MouseDownPrevious == false) // Only add if its a new click
					{
						// I dont like the repeated check but i have to do it
						m_SelectedNode = GetNodeOverlap(clippedMousePos, viewScale, viewOffset);
						m_DraggingNode = m_SelectedNode;

						if (m_DraggingNode == -1)
						{
							m_Nodes.push_back(TransferNode((Uint8)(mousePos.x * 255), mousePos.y));
							m_Dirty = true;
						}
					}
				}
				else if (io.MouseDown[1] == true)
				{
					m_SelectedNode = GetNodeOverlap(clippedMousePos, viewScale, viewOffset);

					if (m_SelectedNode != -1)
					{
						m_Nodes.erase(m_Nodes.begin() + m_SelectedNode);
						m_Dirty = true;

						m_SelectedNode = -1;
						m_DraggingNode = -1;
					}
				}
				else
				{
					m_DraggingNode = -1;
				}
			}
			else
			{
				m_DraggingNode = -1;
			}


			//--Build point list--
			std::vector<ImVec2> pointList;
			for (size_t i = 0; i < m_Nodes.size(); i++)
			{
				TransferNode node = m_Nodes[i];
				// Get normalized iso/alpha per, GetPosition should have better name?
				Vector2 point = Vector2(node.GetIntensity() * 0.00392156862f, node.GetOpacity());
				ImVec2 p = ImVec2(point.x * viewScale.x + viewOffset.x, point.y * viewScale.y + viewOffset.y);
				pointList.push_back(p);
				ImU32 color = (((int)(255) & 0xff) << 24) + (((int)(node.B * 255) & 0xff) << 16) + (((int)(node.G * 255) & 0xff) << 8) + ((int)(node.R * 255) & 0xff);
				drawList->AddCircleFilled(p, 10, color);
			}

			drawList->AddPolyline(pointList.data(), (int)pointList.size(), 0xFFFFFFFF, false, 2.0f);
			drawList->PopClipRect();

			ImGui::Separator();
			if (m_SelectedNode != -1)
			{
				TransferNode& node = m_Nodes[m_SelectedNode];
				float color[3] = { node.R, node.G, node.B };
				if (ImGui::ColorEdit3("Diffuse", color))
				{
					node.R = color[0];
					node.G = color[1];
					node.B = color[2];

					m_Dirty = true;
				}

				int intensity = node.Intensity;
				if (ImGui::SliderInt("Intensity", &intensity, m_Nodes[Mathf::Clamp(m_SelectedNode - 1, 0, (int)m_Nodes.size())].Intensity, Mathf::Min(m_Nodes[Mathf::Clamp(m_SelectedNode + 1, 0, (int)m_Nodes.size() - 1)].Intensity, 255)))
				{
					node.Intensity = (Uint8)intensity;
					m_Dirty = true;
				}

				if (ImGui::SliderFloat("Alpha", &node.Opacity, 0.0f, 1.0f))
				{
					m_Dirty = true;
				}

				if (ImGui::SliderFloat("Metallic", &node.Metallic, 0.0f, 1.0f))
				{
					m_Dirty = true;
				}

				if (ImGui::SliderFloat("Roughness", &node.Roughness, 0.0f, 1.0f))
				{
					m_Dirty = true;
				}
			}

			// cache mouse
			m_MouseDownPrevious = io.MouseDown[0];

			if (m_Dirty)
			{
				// sort points based on the x coordinated
				std::sort(m_Nodes.begin(), m_Nodes.end(), &TransferNode::Sorter);
				GenerateTransferFunction();
				m_Dirty = false;

				// only return edited if its dirty and we finished?
				return m_MouseDownPrevious == false;
			}

			return false;
		}
	}

	int GetNodeOverlap(Vector2 mousePosition, Vector2 viewScale, Vector2 viewOffset)
	{
		for (int i = 0; i < m_Nodes.size(); i++)
		{
			Vector2 point = Vector2(m_Nodes[i].GetIntensity() * 0.00392156862f, m_Nodes[i].GetOpacity()) * viewScale + viewOffset;
			float dist = (point - mousePosition).LengthSquared();

			if (dist <= 10 * 10)
			{
				return i;
			}
		}

		return -1;
	}
};