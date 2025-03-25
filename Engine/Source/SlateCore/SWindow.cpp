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
    Rect = InRect;
    
    FRect ChildRect = {
        Rect.Left + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    
    if (Viewport)
    {
        Viewport->Init(ChildRect);   
    }
}

void SWindow::Tick(const float DeltaTime)
{
    SCompoundWidget::Tick(DeltaTime);
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

SSplitter::~SSplitter()
{
    delete SideLT;
    delete SideRB;
}

void SSplitter::Init(const FRect& InRect)
{
    Rect = InRect;
    
    FRect ChildRect = {
        Rect.Left + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    
    SideLT = new SWindow();
    SideLT->Init(ChildRect);
    
    SideRB = nullptr;
}

void SSplitter::Tick(const float DeltaTime)
{
    SWindow::Tick(DeltaTime);
}

bool SSplitter::IsHover(FPoint coord) const
{
    return SWindow::IsHover(coord);
}

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

void SSplitterH::Split()
{
    if (SideLT)
    {
        delete SideLT;
    }
    if (SideRB)
    {
        delete SideRB;
    }

    int32 HalfWidth = (Rect.Right - Rect.Left) / 2;
    
    FRect LeftRect = {
        Rect.Left + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Left + HalfWidth - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    SideLT = new SSplitterV();
    SideLT->Padding.Right = 5;
    SideLT->Init(LeftRect);

    FRect RightRect = {
        Rect.Left + HalfWidth + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    SideRB = new SSplitterV();
    SideRB->Padding.Left = 5;
    SideRB->Init(RightRect);
}

void SSplitterV::Split()
{
    if (SideLT)
    {
        delete SideLT;
    }
    if (SideRB)
    {
        delete SideRB;
    }

    int32 HalfHeight = (Rect.Bottom - Rect.Top) / 2;
    
    FRect TopRect = {
        Rect.Left + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Top + HalfHeight - Padding.Bottom
    };
    SideLT = new SSplitterH();
    SideLT->Padding.Bottom = 5;
    SideLT->Init(TopRect);

    FRect BottomRect = {
        Rect.Left + Padding.Left,
        Rect.Top + HalfHeight + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    SideRB = new SSplitterH();
    SideRB->Padding.Top = 5;
    SideRB->Init(BottomRect);
}
