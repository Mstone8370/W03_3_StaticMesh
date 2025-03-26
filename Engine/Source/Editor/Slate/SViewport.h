#pragma once

#pragma once
#include "Math/FRect.h"


struct FRenderContext;
class UWorld;
class URenderer;
class FViewport;

class SViewport
{
public:
    SViewport();
    ~SViewport();

    void Tick(float DeltaTime);
    void Render(const FRenderContext& Context);

    void SetRect(const FRect& InRect);
    const FRect& GetRect() const { return Rect; }

    void SetFViewport(FViewport* InViewport);
    FViewport* GetFViewport() const;

    void InitializeFViewport(ID3D11Device* Device);
    bool IsHover(const FPoint& MousePos) const;
    void UpdateFViewportSize();

private:
    FRect Rect;
    FViewport* Viewport = nullptr;
};