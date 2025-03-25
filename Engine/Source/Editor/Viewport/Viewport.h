#pragma once
#include "Container/String.h"
#include "Math/Vector.h"

#pragma once

#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include <d3d11.h>

class ACamera;

/**
 * 하나의 뷰포트를 구성하는 구조체.
 */
struct FViewport
{
    FString Name;

    FVector2D Position; // 뷰포트의 화면 내 위치
    FVector2D Size; // 뷰포트의 크기

    D3D11_VIEWPORT ViewportDesc = {};

    ID3D11Texture2D* RenderTarget = nullptr;
    ID3D11RenderTargetView* RTV = nullptr;

    ID3D11Texture2D* DepthStencil = nullptr;
    ID3D11DepthStencilView* DSV = nullptr;

    ACamera* ViewCamera = nullptr;
    ID3D11ShaderResourceView* ShaderResourceView = nullptr;
    
    //픽킹 전용 텍스처
    ID3D11Texture2D* PickingTexture = nullptr;
    ID3D11RenderTargetView* PickingRTV = nullptr;
    ID3D11Texture2D* PickingStaging = nullptr;
    ID3D11Texture2D* PickingDepth = nullptr;
    ID3D11DepthStencilView* PickingDSV = nullptr;
    ID3D11ShaderResourceView* PixelShaderResourceView = nullptr;
    
    void Initialize(ID3D11Device* Device, float InWidth, float InHeight, const FVector2D& InTopLeft);
    void Release();
};
