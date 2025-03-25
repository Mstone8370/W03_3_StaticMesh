#include "pch.h"
#include "SWindow.h"

#include "SViewport.h"

SWindow::SWindow()
{
}

SWindow::~SWindow()
{
    if (OwnedViewport)
    {
        delete OwnedViewport;
        OwnedViewport = nullptr;
    }
}

void SWindow::Tick(float DeltaTime)
{
    if (OwnedViewport)
    {
        OwnedViewport->Tick(DeltaTime);
    }
}

void SWindow::Render(const FRenderContext& Context)
{
    if (OwnedViewport)
    {
        OwnedViewport->Render(Context);
    }
}



bool SWindow::IsHover(const FPoint& Point) const
{
    return Rect.Contains(Point);
}

void SWindow::SetRect(const FRect& InRect)
{
    Rect = InRect;
    if (OwnedViewport)
    {
        OwnedViewport->SetRect(InRect);
    }
}

const FRect& SWindow::GetRect() const
{
    return Rect;
}

void SWindow::SetViewport(SViewport* InViewport)
{
    OwnedViewport = InViewport;
}

SViewport* SWindow::GetViewport() const
{
    return OwnedViewport;
}