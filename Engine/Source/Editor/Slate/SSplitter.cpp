#include "pch.h"
#include "SSplitter.h"

SSplitter::SSplitter(EOrientation InOrientation)
    : Orientation(InOrientation), SideLT(nullptr), SideRB(nullptr), Ratio(0.5f)
{
}

void SSplitter::SetChildren(SWindow* InSideLT, SWindow* InSideRB)
{
    SideLT = InSideLT;
    SideRB = InSideRB;
}

void SSplitter::SetRatio(float InRatio)
{
    Ratio = FMath::Clamp(InRatio, 0.1f, 0.9f);
}

void SSplitter::Tick(float DeltaTime)
{
    UpdateChildRects();

    if (SideLT) SideLT->Tick(DeltaTime);
    if (SideRB) SideRB->Tick(DeltaTime);
}

void SSplitter::Render(const FRenderContext& Context)
{
    UpdateChildRects();

    if (SideLT) SideLT->Render(Context);
    if (SideRB) SideRB->Render(Context);
}

void SSplitter::UpdateChildRects()
{
    if (!SideLT || !SideRB) return;

    const FRect& MyRect = GetRect();

    if (Orientation == EOrientation::Horizontal)
    {
        float SplitX = MyRect.X + MyRect.Width * Ratio;

        FRect LeftRect(MyRect.X, MyRect.Y, MyRect.Width * Ratio, MyRect.Height);
        FRect RightRect(SplitX, MyRect.Y, MyRect.Width * (1.f - Ratio), MyRect.Height);

        SideLT->SetRect(LeftRect);
        SideRB->SetRect(RightRect);
    }
    else // Vertical
    {
        float SplitY = MyRect.Y + MyRect.Height * Ratio;

        FRect TopRect(MyRect.X, MyRect.Y, MyRect.Width, MyRect.Height * Ratio);
        FRect BottomRect(MyRect.X, SplitY, MyRect.Width, MyRect.Height * (1.f - Ratio));

        SideLT->SetRect(TopRect);
        SideRB->SetRect(BottomRect);
    }
}
