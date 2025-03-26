#pragma once
struct FMatrix;
struct FVector;
#include "Engine/Engine.h"

// 한 프레임 동안 렌더할 각 오브젝트마다 바뀌는 값
struct alignas(16) FCbChangeEveryObject
{
	FMatrix WorldMatrix;
	FMatrix NormalMatrix;
	FVector4 CustomColor;
	// true인 경우 Vertex Color를 사용하고, false인 경우 CustomColor를 사용합니다.
	uint32 bUseVertexColor;
	uint32 Padding[3];
};

// 한 프레임에 한번 바뀌는 값
struct alignas(16) FCbChangeOnCameraMove
{
	FMatrix ViewMatrix;
	FVector ViewPosition;
};

// 화면 크기가 바뀌거나 FOV값이 바뀌는 특정 상황에만 바뀌는 값
struct alignas(16) FCbChangeOnResizeAndFov
{
	FMatrix ProjectionMatrix;
	float NearClip;
	float FarClip;
};

struct alignas(16) FPickingConstants
{
	FVector4 UUIDColor;
};

struct alignas(16) FTextureConstants
{
	FMatrix WorldViewProj;
	float U;
	float V;
	float Cols;
	float Rows;
	int bIsText;
};

struct ConstantUpdateInfo
{
	const FMatrix& TransformMatrix;
	const FVector4& Color;
	bool bUseVertexColor;
};

struct FVertexGrid
{
	FVector Location;
};

struct FMaterialInfo
{
	int ActiveTextureFlag;
	float Ns; // Specular exponent (광택 정도)
	float d; // Dissolve (투명도; 1.0이면 불투명)
	float illum; // Illumination model (조명 모델 번호)
	float Ni; // Optical density 
	FVector Ka; // Ambient color 
	float pad0; // 패딩: float3 뒤에 float 1개 추가
	FVector Kd; // Diffuse color 
	float pad1;
	FVector Ks; // Specular color 
	float pad2;
	FVector Ke; // Emissive color
	float pad3;

	FMaterialInfo()
		: Ns(1.0f)
		, Ka(1.0f, 1.0f, 1.0f)
		, Kd(1.0f, 1.0f, 1.0f)
		, Ks(1.0f, 1.0f, 1.0f)
		, Ke(0.0f, 0.0f, 0.0f)
		, Ni(1.f)
		, d(1.0f)
		, illum()
	{
	}
};

struct FLightingConstants
{
	FVector LightDir;
	float pad4;
	FVector LightColor;
	float pad5;
	FVector AmbientColor;
	float pad6;
	FVector CameraPosition;
	float pad7;
};

struct FCbComposite
{
	FVector2D ViewportSize;
	FVector2D ViewportPosition;
};
