#include "pch.h"
#include "Viewport.h"
#include <d3d11.h>

void FViewport::Initialize(ID3D11Device* Device, float InWidth, float InHeight, const FVector2D& InTopLeft)
{
    // Render Target 생성
    D3D11_TEXTURE2D_DESC RTDesc = {};
    RTDesc.Width = static_cast<UINT>(InWidth);
    RTDesc.Height = static_cast<UINT>(InHeight);
    RTDesc.MipLevels = 1;
    RTDesc.ArraySize = 1;
    RTDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    RTDesc.SampleDesc.Count = 1;
    RTDesc.Usage = D3D11_USAGE_DEFAULT;
    RTDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

    Device->CreateTexture2D(&RTDesc, nullptr, &RenderTarget);
    Device->CreateRenderTargetView(RenderTarget, nullptr, &RTV);

    // Depth Stencil 생성
    D3D11_TEXTURE2D_DESC DSDesc = RTDesc;
    DSDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DSDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Device->CreateTexture2D(&DSDesc, nullptr, &DepthStencil);
    Device->CreateDepthStencilView(DepthStencil, nullptr, &DSV);

    // Viewport 설정
    ViewportDesc.TopLeftX = InTopLeft.X;
    ViewportDesc.TopLeftY = InTopLeft.Y;
    ViewportDesc.Width = InWidth;
    ViewportDesc.Height = InHeight;
    ViewportDesc.MinDepth = 0.0f;
    ViewportDesc.MaxDepth = 1.0f;
}


void FViewport::Release()
{
    if (RenderTarget) { RenderTarget->Release(); RenderTarget = nullptr; }
    if (RTV) { RTV->Release(); RTV = nullptr; }
    if (DepthStencil) { DepthStencil->Release(); DepthStencil = nullptr; }
    if (DSV) { DSV->Release(); DSV = nullptr; }
}
