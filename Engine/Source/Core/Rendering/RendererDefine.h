#pragma once
#include "Core/HAL/PlatformType.h"
#include "Math/Vector.h"

enum class EViewModeIndex : uint8
{
	ERS_Solid,
	ERS_Wireframe,

	ERS_Max,
};

// !TODO : 추후에 RTTI등으로 변경
enum class EPrimitiveType : uint8
{
	EPT_None,
	EPT_Triangle,
	EPT_Cube,
	EPT_Sphere,
	EPT_Line,
	EPT_Cylinder,
	EPT_Cone,
	EPT_Max,
};

enum class EIndexBufferType : uint8
{
	EIT_Cube,
};
struct FStaticMeshVertex
{
	FVector   Position;  // 16바이트 할당(실제 크기 12바이트 + 4바이트 내부 패딩)
	FVector   Normal;    // 16바이트 할당
	FVector   Tangent;   // 16바이트 할당
	FVector2D UV;        // 8바이트
	FVector4  Color;     // 16바이트
};