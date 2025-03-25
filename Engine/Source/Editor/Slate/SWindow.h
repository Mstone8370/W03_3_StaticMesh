#pragma once
#include "Container/Array.h"
#include "Math/FRect.h"

struct FRenderContext;
class URenderer;
class UWorld;
class SViewport;

class SWindow
{
public:
    SWindow();
    ~SWindow();

    virtual void Tick(float DeltaTime);
    virtual void Render(const FRenderContext& Context);

    void SetRect(const FRect& InRect);
    const FRect& GetRect() const;

    bool IsHover(const FPoint& Point) const;

    void SetViewport(SViewport* InViewport);
    SViewport* GetViewport() const;

private:
    FRect Rect;
    SViewport* OwnedViewport = nullptr;
};
