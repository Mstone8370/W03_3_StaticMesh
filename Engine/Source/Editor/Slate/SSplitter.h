#pragma once
#include "SWindow.h"

class SSplitter : public SWindow
{
public:
    enum class EOrientation { Horizontal, Vertical };

    SSplitter(EOrientation InOrientation);

    void SetChildren(SWindow* First, SWindow* Second);
    void SetRatio(float InRatio);
    void Tick(float DeltaTime) override;
    virtual void Render(const FRenderContext& Context) override;
    void UpdateChildRects();

protected:
    SWindow* SideLT = nullptr;
    SWindow* SideRB = nullptr;
    float Ratio = 0.5f;
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