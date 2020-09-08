#pragma once
#include "Graphics.h"
namespace CommonStates
{
	//--Common Descriptors--
	const BlendDesc Opaque =
	{
		false, // AlphaToCoverageEnable
		false, // IndependentBlendEnable
		{ {
			false,        // BlendEnable
			Blend::One,   // SrcBlend
			Blend::Zero,  // DestBlend
			BlendOp::Add, // BlendOp
			Blend::One,   // SrcBlendAlpha
			Blend::Zero,  // DestBlendAlpha
			BlendOp::Add, // BlendOpAlpha
			(Int16)ColorWrite::All
		} }
	};

	const BlendDesc AlphaBlend =
	{
		false, // AlphaToCoverageEnable
		false, // IndependentBlendEnable
		{ {
			true,	              // BlendEnable
			Blend::One,           // SrcBlend
			Blend::Inv_Src_Alpha, // DestBlend
			BlendOp::Add,		  // BlendOp
			Blend::One,			  // SrcBlendAlpha
			Blend::Inv_Src_Alpha, // DestBlendAlpha
			BlendOp::Add,	      // BlendOpAlpha
			(Int16)ColorWrite::All
		} }
	};

	const BlendDesc Additive =
	{
		false, // AlphaToCoverageEnable
		false, // IndependentBlendEnable
		{ {
			true, // BlendEnable
			Blend::Src_Alpha, // SrcBlend
			Blend::One,	      // DestBlend
			BlendOp::Add,     // BlendOp
			Blend::Src_Alpha, // SrcBlendAlpha
			Blend::One,       // DestBlendAlpha
			BlendOp::Add,     // BlendOpAlpha
			(Int16)ColorWrite::All
		} }
	};

	const BlendDesc NonPremultiplied =
	{
		false, // AlphaToCoverageEnable
		false, // IndependentBlendEnable
		{ {
			true,		          // BlendEnable
			Blend::Src_Alpha,	  // SrcBlend
			Blend::Inv_Src_Alpha, // DestBlend
			BlendOp::Add,		  // BlendOp
			Blend::Src_Alpha,     // SrcBlendAlpha
			Blend::Inv_Src_Alpha, // DestBlendAlpha
			BlendOp::Add,        // BlendOpAlpha
			(Int16)ColorWrite::All
		} }
	};

	const DepthDesc DepthNone =
	{
		false,						 // DepthEnable
		DepthWrite::Zero,
		ComparisonFunc::LessEqual, // DepthFunc
		false,                       // StencilEnable
		0xFF,
		0xFF,
		{
			StencilOp::Keep,        // StencilFailOp
			StencilOp::Keep,        // StencilDepthFailOp
			StencilOp::Keep,        // StencilPassOp
			ComparisonFunc::Always  // StencilFunc
		}, // FrontFace
		{
			StencilOp::Keep,        // StencilFailOp
			StencilOp::Keep,        // StencilDepthFailOp
			StencilOp::Keep,        // StencilPassOp
			ComparisonFunc::Always  // StencilFunc
		} // BackFace
	};

	const DepthDesc DepthDefault =
	{
		true, // DepthEnable
		DepthWrite::All,
		ComparisonFunc::LessEqual, // DepthFunc
		false, // StencilEnable
		0xFF,
		0xFF,
		{
			StencilOp::Keep,       // StencilFailOp
			StencilOp::Keep,       // StencilDepthFailOp
			StencilOp::Keep,       // StencilPassOp
			ComparisonFunc::Always // StencilFunc
		}, // FrontFace
		{
			StencilOp::Keep,       // StencilFailOp
			StencilOp::Keep,       // StencilDepthFailOp
			StencilOp::Keep,       // StencilPassOp
			ComparisonFunc::Always // StencilFunc
		} // BackFace
	};

	const DepthDesc DepthRead =
	{
		true, // DepthEnable
		DepthWrite::Zero,
		ComparisonFunc::LessEqual, // DepthFunc
		false, // StencilEnable
		0xFF,
		0xFF,
		{
			StencilOp::Keep,       // StencilFailOp
			StencilOp::Keep,       // StencilDepthFailOp
			StencilOp::Keep,       // StencilPassOp
			ComparisonFunc::Always // StencilFunc
		}, // FrontFace
		{
			StencilOp::Keep,       // StencilFailOp
			StencilOp::Keep,       // StencilDepthFailOp
			StencilOp::Keep,       // StencilPassOp
			ComparisonFunc::Always // StencilFunc
		} // BackFace
	};

	const RasterDesc CullNone =
	{
		FillMode::Solid,
		CullMode::None,
		false, // FrontCounterClockwise
		0,
		0.0f,
		0.0f,
		true, // DepthClipEnable
		true, // MultisampleEnable
		false, // AntialiasedLineEnable
	};

	const RasterDesc CullClockwise =
	{
		FillMode::Solid,
		CullMode::Front,
		false, // FrontCounterClockwise
		0,
		0.0f,
		0.0f,
		true, // DepthClipEnable
		true, // MultisampleEnable
		false, // AntialiasedLineEnable
	};

	const RasterDesc CullCounterClockwise =
	{
		FillMode::Solid,
		CullMode::Back,
		false, // FrontCounterClockwise
		0,
		0.0f,
		0.0f,
		true, // DepthClipEnable
		true, // MultisampleEnable
		false, // AntialiasedLineEnable
	};

	const RasterDesc Wireframe =
	{
		FillMode::Wireframe,
		CullMode::None,
		false, // FrontCounterClockwise
		0,
		0.0f,
		0.0f,
		true, // DepthClipEnable
		true, // MultisampleEnable
		false, // AntialiasedLineEnable
	};


	const SamplerDesc PointWrap =
	{
		FilterMode::MinMagMipPoint,
		WrapMode::Repeat, // AddressU
		WrapMode::Repeat, // AddressV
		WrapMode::Repeat, // AddressW
		0, // MipLODBias
		0,
		ComparisonFunc::Never,
		{0, 0, 0, 1},
		0, // MinLOD
		FLT_MAX, // MaxLOD
	};

	const SamplerDesc PointClamp =
	{
		FilterMode::MinMagMipPoint,
		WrapMode::Clamp, // AddressU
		WrapMode::Clamp, // AddressV
		WrapMode::Clamp, // AddressW
		0, // MipLODBias
		0,
		ComparisonFunc::Never,
		{0, 0, 0, 1},
		0, // MinLOD
		FLT_MAX, // MaxLOD
	};

	const SamplerDesc LinearWrap =
	{
		FilterMode::MinMagMipLinear,
		WrapMode::Repeat, // AddressU
		WrapMode::Repeat, // AddressV
		WrapMode::Repeat, // AddressW
		0, // MipLODBias
		0,
		ComparisonFunc::Never,
		{0, 0, 0, 1},
		0, // MinLOD
		FLT_MAX, // MaxLOD
	};

	const SamplerDesc LinearClamp =
	{
		FilterMode::MinMagMipLinear,
		WrapMode::Clamp, // AddressU
		WrapMode::Clamp, // AddressV
		WrapMode::Clamp, // AddressW
		0, // MipLODBias
		0,
		ComparisonFunc::Never,
		{0, 0, 0, 1},
		0, // MinLOD
		FLT_MAX, // MaxLOD
	};

	const SamplerDesc AnisotropicWrap =
	{
		FilterMode::Anisotropic,
		WrapMode::Repeat, // AddressU
		WrapMode::Repeat, // AddressV
		WrapMode::Repeat, // AddressW
		0, // MipLODBias
		0,
		ComparisonFunc::Never,
		{0, 0, 0, 1},
		0, // MinLOD
		FLT_MAX, // MaxLOD
	};

	const SamplerDesc AnisotropicClamp =
	{
		FilterMode::Anisotropic,
		WrapMode::Clamp, // AddressU
		WrapMode::Clamp, // AddressV
		WrapMode::Clamp, // AddressW
		0, // MipLODBias
		0,
		ComparisonFunc::Never,
		{0, 0, 0, 1},
		0, // MinLOD
		FLT_MAX, // MaxLOD
	};
};