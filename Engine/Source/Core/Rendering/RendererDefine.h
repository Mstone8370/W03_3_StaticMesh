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

//struct FNormalVertex
//{
//	FVector Position;
//	FVector Normal;
//	FVector Tangent;
//	FVector2D UV;
//};

struct FNormalVertex
{
    FVector Position;
    FVector Normal;
    FVector4 Color;
    FVector2D UV;
    FVector Tangent;
};

// Cooked Data
struct FStaticMesh
{
    FString PathFileName;

    TArray<FNormalVertex> Vertices;
    TArray<uint32> Indices;
};

// Raw Data
struct FObjInfo
{
    TArray<TArray<float>> Vertices;
    TArray<TArray<float>> UVs;
    TArray<TArray<float>> Normals;
    TArray<TArray<uint32>> VertexIndices;
    TArray<TArray<uint32>> UVIndices;
    TArray<TArray<uint32>> NormalIndices;

    // Material List
    // Texture List
};

struct FObjMaterialInfo
{
    // Diffuse Scalar
    // Diffuse Texture
};

struct FObjSubMesh
{
    FString MaterialName;
    uint32 StartIdx;
    uint32 EndIdx;
};