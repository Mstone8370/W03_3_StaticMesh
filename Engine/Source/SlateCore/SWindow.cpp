#include "pch.h"
#include "SWindow.h"
#include "Input/PlayerInput.h"

SWindow::SWindow()
    : Viewport(std::make_unique<SViewport>())
{}

SWindow::~SWindow()
{
    if (Viewport)
    {
        Viewport.reset();
    }
}

void SWindow::Init(const FRect& InRect)
{
    SCompoundWidget::Init(InRect);
}

bool SWindow::IsHover(FPoint coord) const
{
    return Rect.Left < coord.X && coord.X < Rect.Right && Rect.Top < coord.Y && coord.Y < Rect.Bottom;
}

void SWindow::Paint()
{
    if (Viewport)
    {
        Viewport->Paint();
    }
}

void SWindow::HandleInput(const float DeltaTime)
{
    if (!Viewport)
    {
        return;
    }
    
    FPoint Point = APlayerInput::Get().GetMousePositionClient();
    // 마우스 포인터가 이 윈도우에 호버링 중일때만 입력을 처리 
    if (IsHover(Point))
    {
        Viewport->HandleInput(DeltaTime);
    }
}


SSplitter::SSplitter()
    : SideLT(nullptr)
    , SideRB(nullptr)
{}

void SSplitter::Paint()
{
    if (SideLT)
    {
        SideLT->Paint();
    }
    if (SideRB)
    {
        SideRB->Paint();
    }
}

void SSplitter::HandleInput(const float DeltaTime)
{
    FPoint Point = APlayerInput::Get().GetMousePositionClient();
    if (!IsHover(Point))
    {
        return;
    }
    
    if (SideLT && SideRB)
    {
        if (SideLT->IsHover(Point) && SideRB->IsHover(Point))
        {
            // TODO: 둘 다 호버인 경우는 발생하면 안됨.
        }
        
        if (SideLT->IsHover(Point))
        {
            SideLT->HandleInput(DeltaTime);
        }
        else if (SideRB->IsHover(Point))
        {
            SideRB->HandleInput(DeltaTime);
        }
        else
        {
            // TODO: 이 경우는 분리된 화면의 크기를 조절하는 입력일 수 있음.
        }
    }
    else
    {
        // Something went wrong...
        if (SideLT && SideLT->IsHover(Point))
        {
            SideLT->HandleInput(DeltaTime);
        }
        if (SideRB && SideRB->IsHover(Point))
        {
            SideRB->HandleInput(DeltaTime);
        }
    }
}
