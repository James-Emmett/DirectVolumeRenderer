#include "Graphics/VertexTypes.h"

//----------------------------------------------------------------------------
const InputElement Vertex::InputLayout[1] =
{
	{ "POSITION", 0, 0, SurfaceFormat::R32G32B32_Float, InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 }
};

//-----------------------------------------------------------------------------
const InputElement VertexTexture::InputLayout[2] =
{
	{ "POSITION"   , 0, 0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "TEXCOORD"   , 0, 0, SurfaceFormat::R32G32_Float      , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 }
};

//-----------------------------------------------------------------------------
const InputElement VertexColorTexture::InputLayout[3] =
{
	{ "POSITION"   , 0, 0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "COLOR"      , 0, 0, SurfaceFormat::R32G32B32A32_Float, InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "TEXCOORD"   , 0, 0, SurfaceFormat::R32G32_Float      , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 }
};

//-----------------------------------------------------------------------------
const InputElement VertexNormal::InputLayout[4] =
{
	{ "POSITION"   , 0,  0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0},
	{ "NORMAL"     , 0,  0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0},
	{ "COLOR"      , 0,  0, SurfaceFormat::R32G32B32A32_Float, InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0},
	{ "TEXCOORD"   , 0,  0, SurfaceFormat::R32G32_Float      , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0}
};

//-----------------------------------------------------------------------------
const InputElement VertexMesh::InputLayout[6] =
{
	{ "POSITION", 0, 0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "NORMAL"  , 0, 0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "TANGENT" , 0, 0, SurfaceFormat::R32G32B32_Float, InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "COLOR"   , 0, 0, SurfaceFormat::R32G32B32A32_Float, InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "TEXCOORD", 0, 0, SurfaceFormat::R32G32_Float      , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 }
};

//-----------------------------------------------------------------------------
const InputElement VertexSkimmedMesh::InputLayout[7] =
{
	{ "POSITION"     , 0, 0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "NORMAL"       , 0, 0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "TANGENT"      , 0, 0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "COLOR"        , 0, 0, SurfaceFormat::R32G32B32A32_Float, InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "TEXCOORD"     , 0, 0, SurfaceFormat::R32G32_Float      , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "BLENDINDICES" , 0, 0, SurfaceFormat::R32G32B32A32_Uint , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
	{ "BLENDWEIGHT"  , 0, 0, SurfaceFormat::R32G32B32_Float   , InputElement::ALIGNED_ELEMENT, InputType::VertexData, 0 },
};