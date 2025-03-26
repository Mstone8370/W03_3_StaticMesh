#pragma once
#include "SWindow.h"

class SSplitter : public SWindow
{
public:
    enum class EOrientation { Horizontal, Vertical };

    SSplitter(EOrientation InOrientation);

    void SetChildren(SWindow* First, SWindow* Second);
    void SetChild(int Index, SWindow* Child);
    SWindow* GetChild(int Index) const;
    void SetRatio(float InRatio);
    float GetRatio(){return Ratio;};
    void SetViewportPadding(float InPadding);

    void Tick(float DeltaTime) override;
    virtual void Render(const FRenderContext& Context) override;
    void UpdateChildRects();

protected:
    SWindow* SideLT = nullptr;
    SWindow* SideRB = nullptr;
    float Ratio = 0.5f;
    float ViewportPadding  = 2.f; // px 단위
    EOrientation Orientation;
};

// 세로 스플리터
class SSplitterH : public SSplitter
{
public:
    SSplitterH() : SSplitter(EOrientation::Horizontal) {}
};

// 가로 스플리터
class SSplitterV : public SSplitter
{
public:
    SSplitterV() : SSplitter(EOrientation::Vertical) {}
};