#include "pch.h"
#include "SViewport.h"

#include "Editor/Viewport/FViewport.h"
#include "Editor/Viewport/FViewportClient.h"

SViewport::SViewport()
{
    Viewport = new FViewport();
}

SViewport::~SViewport()
{
    if (Viewport)
    {
        Viewport->Release();
        delete Viewport;
        Viewport = nullptr;
    }
}

void SViewport::Tick(float DeltaTime)
{
    if (!Viewport)
        return; 

    // 여기에 더 필요한 Tick 동작이 있다면 추가 가능 (예: 애니메이션, Gizmo 등)
}

void SViewport::Render(const FRenderContext& Context)
{
    if (Viewport)
    {
        Viewport->Render(Context);
    }
}

void SViewport::SetFViewport(FViewport* InViewport)
{
    Viewport = InViewport;
}

FViewport* SViewport::GetFViewport() const
{
    return Viewport;
}
void SViewport::InitializeFViewport(ID3D11Device* Device)
{
    if (!Viewport)
        return;

    const float Width = Rect.Width;
    const float Height = Rect.Height;

    Viewport->Initialize(Device, Width, Height);
}
bool SViewport::IsHover(const FPoint& MousePos) const
{
    return Rect.Contains(MousePos);
}
void SViewport::UpdateFViewportSize()
{
    if (!Viewport)
        return;

    const float NewWidth = Rect.Width;
    const float NewHeight = Rect.Height;
    Viewport->Resize(NewWidth, NewHeight);
}
