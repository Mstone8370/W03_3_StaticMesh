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

void SWindow::OnResize(const FRect& NewRect)
{
    Rect = NewRect;

    FRect ChildRect = {
        Rect.Left + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    if (Viewport)
    {
        Viewport->OnResize(ChildRect);
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
    PercentageLT = 1.f;
    
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

void SSplitter::OnResize(const FRect& NewRect)
{
    /**
     * 이 클래스의 함수는 기초적인 작동만 진행중.
     * 현재 두 children이 어느 크기를 가지고있었는지 상관 없이 하나의 child가 꽉 차게 진행함.
     * 하위 클래스인 SSplitterH 또는 SSplitterV에서는 상황에 따른 정확한 사이즈 조절이 가능함.
     */
    FRect PrevRect = Rect;
    Rect = NewRect;

    FRect ChildRect = {
        Rect.Left + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    if (SideLT)
    {
        FRect LTRect = SideLT->Rect;
        SideLT->OnResize(ChildRect);
    }
    if (SideRB)
    {
        FRect RBRect = SideRB->Rect;
        SideRB->OnResize(ChildRect);
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

    PercentageLT = 0.5f;
    int32 ParentWidth = (Rect.Right - Rect.Left);
    int32 WidthLT = ParentWidth * PercentageLT;
    int32 WidthRB = ParentWidth - WidthLT;
    
    FRect LeftRect = {
        Rect.Left + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Left + WidthLT - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    SideLT = new SSplitterV();
    SideLT->Padding.Right = 5;
    SideLT->Init(LeftRect);

    FRect RightRect = {
        Rect.Right - WidthRB + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    SideRB = new SSplitterV();
    SideRB->Padding.Left = 5;
    SideRB->Init(RightRect);
}

void SSplitterH::OnResize(const FRect& NewRect)
{
    FRect PrevRect = Rect;
    Rect = NewRect;

    int32 NewParentWidth = Rect.Right - Rect.Left;
    int32 WidthLT = NewParentWidth * PercentageLT;
    int32 WidthRB = NewParentWidth - WidthLT;
    
    if (SideLT)
    {
        FRect ChildRect = {
            Rect.Left + Padding.Left,
            Rect.Top + Padding.Top,
            Rect.Left + WidthLT - Padding.Right,
            Rect.Bottom - Padding.Bottom
        };
        SideLT->OnResize(ChildRect);
    }
    if (SideRB)
    {
        FRect ChildRect = {
            Rect.Right - WidthRB + Padding.Left,
            Rect.Top + Padding.Top,
            Rect.Right + Padding.Right,
            Rect.Bottom - Padding.Bottom
        };
        SideRB->OnResize(ChildRect);
    }
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

    PercentageLT = 0.5f;
    int32 ParentHeight = (Rect.Bottom - Rect.Top);
    int32 HeightLT = ParentHeight * PercentageLT;
    int32 HeightRB = ParentHeight - HeightLT;
    
    FRect TopRect = {
        Rect.Left + Padding.Left,
        Rect.Top + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Top + HeightLT - Padding.Bottom
    };
    SideLT = new SSplitterH();
    SideLT->Padding.Bottom = 5;
    SideLT->Init(TopRect);

    FRect BottomRect = {
        Rect.Left + Padding.Left,
        Rect.Bottom - HeightRB + Padding.Top,
        Rect.Right - Padding.Right,
        Rect.Bottom - Padding.Bottom
    };
    SideRB = new SSplitterH();
    SideRB->Padding.Top = 5;
    SideRB->Init(BottomRect);
}

void SSplitterV::OnResize(const FRect& NewRect)
{
    FRect PrevRect = Rect;
    Rect = NewRect;

    int32 ParentHeight = Rect.Bottom - Rect.Top;
    int32 HeightLT = ParentHeight * PercentageLT;
    int32 HeightRB = ParentHeight - HeightLT;
    
    if (SideLT)
    {
        FRect ChildRect = {
            Rect.Left + Padding.Left,
            Rect.Top + Padding.Top,
            Rect.Right - Padding.Right,
            Rect.Top + HeightLT - Padding.Bottom
        };
        SideLT->OnResize(ChildRect);
    }
    if (SideRB)
    {
        FRect ChildRect = {
            Rect.Left + Padding.Left,
            Rect.Bottom - HeightRB + Padding.Top,
            Rect.Right - Padding.Right,
            Rect.Bottom - Padding.Bottom
        };
        SideRB->OnResize(ChildRect);
    }
}
