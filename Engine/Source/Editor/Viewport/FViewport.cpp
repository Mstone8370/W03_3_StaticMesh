#include "pch.h"
#include "FViewport.h"

#include "FViewportClient.h"
#include "GameFrameWork/Camera.h"
#include "Rendering/RenderContext.h"
#include "Rendering/URenderer.h"
FViewport::FViewport() {}

FViewport::~FViewport()
{
    Release();
}

void FViewport::Initialize(ID3D11Device* InDevice, float InWidth, float InHeight)
{
    Device = InDevice;
    
    // Render Target
    D3D11_TEXTURE2D_DESC RTDesc = {};
    RTDesc.Width = static_cast<UINT>(InWidth);
    RTDesc.Height = static_cast<UINT>(InHeight);
    RTDesc.MipLevels = 1;
    RTDesc.ArraySize = 1;
    RTDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    RTDesc.SampleDesc.Count = 1;
    RTDesc.Usage = D3D11_USAGE_DEFAULT;
    RTDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    Device->CreateTexture2D(&RTDesc, nullptr, &RenderTarget);
    Device->CreateRenderTargetView(RenderTarget, nullptr, &RTV);

    // SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = RTDesc.Format;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;
    Device->CreateShaderResourceView(RenderTarget, &SRVDesc, &ShaderResourceView);

    // Depth Stencil
    D3D11_TEXTURE2D_DESC DSDesc = RTDesc;
    DSDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DSDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    Device->CreateTexture2D(&DSDesc, nullptr, &DepthStencil);
    Device->CreateDepthStencilView(DepthStencil, nullptr, &DSV);

    // Picking
    Device->CreateTexture2D(&RTDesc, nullptr, &PickingTexture);
    Device->CreateRenderTargetView(PickingTexture, nullptr, &PickingRTV);
    Device->CreateShaderResourceView(PickingTexture, &SRVDesc, &PixelShaderResourceView);

    D3D11_TEXTURE2D_DESC StagingDesc = RTDesc;
    StagingDesc.Usage = D3D11_USAGE_STAGING;
    StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    StagingDesc.BindFlags = 0;
    Device->CreateTexture2D(&StagingDesc, nullptr, &PickingStaging);

    D3D11_TEXTURE2D_DESC DepthDesc = RTDesc;
    DepthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    Device->CreateTexture2D(&DepthDesc, nullptr, &PickingDepth);
    Device->CreateDepthStencilView(PickingDepth, nullptr, &PickingDSV);

    // ViewportDesc
    ViewportDesc.TopLeftX = 0;
    ViewportDesc.TopLeftY = 0;
    ViewportDesc.Width = InWidth;
    ViewportDesc.Height = InHeight;
    ViewportDesc.MinDepth = 0.0f;
    ViewportDesc.MaxDepth = 1.0f;
    //ViewCamera->SetOrthoSize(InWidth / 360.f, InHeight / 360.f);
}

void FViewport::Release()
{
    if (RenderTarget) { RenderTarget->Release(); RenderTarget = nullptr; }
    if (RTV) { RTV->Release(); RTV = nullptr; }
    if (DepthStencil) { DepthStencil->Release(); DepthStencil = nullptr; }
    if (DSV) { DSV->Release(); DSV = nullptr; }
    if (ShaderResourceView) { ShaderResourceView->Release(); ShaderResourceView = nullptr; }
    if (PixelShaderResourceView) { PixelShaderResourceView->Release(); PixelShaderResourceView = nullptr; }
    if (PickingTexture) { PickingTexture->Release(); PickingTexture = nullptr; }
    if (PickingRTV) { PickingRTV->Release(); PickingRTV = nullptr; }
    if (PickingStaging) { PickingStaging->Release(); PickingStaging = nullptr; }
    if (PickingDepth) { PickingDepth->Release(); PickingDepth = nullptr; }
    if (PickingDSV) { PickingDSV->Release(); PickingDSV = nullptr; }
}

void FViewport::Render(const FRenderContext& Context)
{
    if (!Context.Renderer || !Context.World || !RTV || !DSV)
        return;

    ID3D11DeviceContext* DeviceContext = Context.Renderer->GetDeviceContext();

    DeviceContext->OMSetRenderTargets(1, &RTV, DSV);
    DeviceContext->ClearRenderTargetView(RTV, Context.Renderer->ViewportClearColor);
    DeviceContext->ClearDepthStencilView(DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    DeviceContext->RSSetViewports(1, &ViewportDesc);

    if (ViewCamera)
    {
        Context.Renderer->UpdateViewMatrix(ViewCamera->GetActorTransform());
        Context.Renderer->UpdateProjectionMatrixAspect(ViewCamera, ViewportDesc.Width, ViewportDesc.Height);
    }

    if (Client)
    {
        Client->Draw(this, Context);
    }
}

void FViewport::Resize(float NewWidth, float NewHeight)
{
    Release(); // 기존 리소스 해제
    Initialize(Device, NewWidth, NewHeight); // 재생성 (Device는 멤버로 보관해두는 걸 권장)
    //ViewCamera->SetOrthoSize(NewWidth / 360.f, NewHeight / 360.f);
}
bool FViewport::IsInBounds(int32 X, int32 Y) const
{
    int32 Left   = static_cast<int32>(ViewportDesc.TopLeftX);
    int32 Top    = static_cast<int32>(ViewportDesc.TopLeftY);
    int32 Right  = Left + static_cast<int32>(ViewportDesc.Width);
    int32 Bottom = Top + static_cast<int32>(ViewportDesc.Height);

    return X >= Left && X < Right && Y >= Top && Y < Bottom;
}
FVector4 FViewport::GetPickingPixel(ID3D11DeviceContext* Context, int32 X, int32 Y)
{
    FVector4 Color = FVector4(1, 1, 1, 1);

    int32 LocalX = X - static_cast<int32>(ViewportDesc.TopLeftX);
    int32 LocalY = Y - static_cast<int32>(ViewportDesc.TopLeftY);

    if (LocalX < 0 || LocalY < 0 ||
        LocalX >= static_cast<int32>(ViewportDesc.Width) ||
        LocalY >= static_cast<int32>(ViewportDesc.Height))
        return Color;

    D3D11_BOX SrcBox = {
        (UINT)LocalX, (UINT)LocalY, 0,
        (UINT)(LocalX + 1), (UINT)(LocalY + 1), 1
    };

    Context->CopySubresourceRegion(PickingStaging, 0, 0, 0, 0, PickingTexture, 0, &SrcBox);

    D3D11_MAPPED_SUBRESOURCE Mapped = {};
    if (FAILED(Context->Map(PickingStaging, 0, D3D11_MAP_READ, 0, &Mapped)))
        return Color;

    const BYTE* PixelData = static_cast<const BYTE*>(Mapped.pData);
    Color.X = static_cast<float>(PixelData[0]);
    Color.Y = static_cast<float>(PixelData[1]);
    Color.Z = static_cast<float>(PixelData[2]);
    Color.W = static_cast<float>(PixelData[3]);

    Context->Unmap(PickingStaging, 0);

    return Color;
}
