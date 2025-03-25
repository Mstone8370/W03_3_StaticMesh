#pragma once
#include "Container/String.h"
#include "Math/Vector.h"

struct FRenderContext;
class UWorld;
struct FPoint;
class FViewportClient;
class ACamera;

#pragma once

#include <d3d11.h>

class FViewportClient;
class ACamera;

class FViewport
{
public:
    FViewport();
    ~FViewport();
    
    FString Name;

    D3D11_VIEWPORT ViewportDesc;

    ID3D11Texture2D* RenderTarget = nullptr;
    ID3D11RenderTargetView* RTV = nullptr;
    ID3D11ShaderResourceView* ShaderResourceView = nullptr;

    ID3D11Texture2D* DepthStencil = nullptr;
    ID3D11DepthStencilView* DSV = nullptr;

    // Picking
    ID3D11Texture2D* PickingTexture = nullptr;
    ID3D11RenderTargetView* PickingRTV = nullptr;
    ID3D11Texture2D* PickingStaging = nullptr;
    ID3D11Texture2D* PickingDepth = nullptr;
    ID3D11DepthStencilView* PickingDSV = nullptr;
    ID3D11ShaderResourceView* PixelShaderResourceView = nullptr;

    ACamera* ViewCamera = nullptr;
    FViewportClient* Client = nullptr;
public:
    void Initialize(ID3D11Device* InDevice, float Width, float Height);
    void Release();
    void Render(const FRenderContext& Context);
    void Resize(float NewWidth, float NewHeight);
    bool IsInBounds(int32 X, int32 Y) const;
    FVector4 GetPickingPixel(ID3D11DeviceContext* Context, int32 X, int32 Y);

    void SetClient(FViewportClient* InClient) { Client = InClient; }
    FViewportClient* GetClient() const { return Client; }

    void SetCamera(ACamera* InCamera) { ViewCamera = InCamera; }
    ACamera* GetCamera() const { return ViewCamera; }
    int32 index=0;
private:
    ID3D11Device* Device = nullptr;

};

