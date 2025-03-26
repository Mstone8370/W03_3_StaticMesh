#include "pch.h"
#include "SSplitter.h"

#include <complex.h>

SSplitter::SSplitter(EOrientation InOrientation)
    : Orientation(InOrientation), SideLT(nullptr), SideRB(nullptr), Ratio(0.5f)
{
}

void SSplitter::SetChildren(SWindow* InSideLT, SWindow* InSideRB)
{
    SideLT = InSideLT;
    SideRB = InSideRB;
}
void SSplitter::SetChild(int Index, SWindow* Child)
{
    if (Index == 0) SideLT = Child;
    else if (Index == 1) SideRB = Child;
}
SWindow* SSplitter::GetChild(int Index) const
{
    if (Index == 0) return SideLT;
    if (Index == 1) return SideRB;
    return nullptr;
}
void SSplitter::SetRatio(float InRatio,bool IgnoreClamp)
{
    if (!IgnoreClamp)
    Ratio = FMath::Clamp(InRatio, 0.01f, 0.99f);
    else  Ratio = InRatio;
}
void SSplitter::SetViewportPadding(float InPadding)
{
    ViewportPadding = InPadding;
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
    const float HalfPad = ViewportPadding * 0.5f;

    if (Orientation == EOrientation::Horizontal)
    {
        float TotalWidth = MyRect.Width;
        float SplitX = MyRect.X + TotalWidth * Ratio;

        float LeftW  = TotalWidth * Ratio - HalfPad;
        float RightW = TotalWidth * (1.f - Ratio) - HalfPad;

        FRect LeftRect(MyRect.X, MyRect.Y, LeftW, MyRect.Height);
        FRect RightRect(SplitX + HalfPad, MyRect.Y, RightW, MyRect.Height);

        SideLT->SetRect(LeftRect);
        SideRB->SetRect(RightRect);
    }
    else // Vertical
    {
        float TotalHeight = MyRect.Height;
        float SplitY = MyRect.Y + TotalHeight * Ratio;

        float TopH    = TotalHeight * Ratio - HalfPad;
        float BottomH = TotalHeight * (1.f - Ratio) - HalfPad;

        FRect TopRect(MyRect.X, MyRect.Y, MyRect.Width, TopH);
        FRect BottomRect(MyRect.X, SplitY + HalfPad, MyRect.Width, BottomH);

        SideLT->SetRect(TopRect);
        SideRB->SetRect(BottomRect);
    }
}
