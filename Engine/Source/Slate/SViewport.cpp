#include "pch.h"
#include "SViewport.h"

SViewport::SViewport()
    : Viewport(std::make_shared<FViewport>())
{}

SViewport::~SViewport()
{
    if (Viewport)
    {
        Viewport.reset();
    }
}

void SViewport::Init(const FRect& InRect)
{
    Rect = InRect;
    if (!Viewport)
    {
        return;
    }

    int32 Width = Rect.Right - Rect.Left;
    int32 Height = Rect.Bottom - Rect.Top;
    Viewport->Init(0, 0, Width, Height);   
}

void SViewport::Paint()
{
    if (Viewport)
    {
        Viewport->Draw();
    }
}

void SViewport::HandleInput(const float DeltaTime)
{
    if (!Viewport)
    {
        return;
    }

    /**
     * 이 위젯의 부모에서 HandleInput을 처리했으므로, 마우스 커서는 부모의 영역 안에 존재한다는 것은 분명함.
     * 하지만 위젯의 상태에 따라 이 위젯에 마우스가 호버되지 않았을 가능성은 충분히 많으므로, 다시 IsHover를 검사한다.
     */
    FPoint Point = APlayerInput::Get().GetMousePositionClient();
    if (IsHover(Point))
    {
        Viewport->HandleInput(DeltaTime);
    }
}

bool SViewport::IsHover(FPoint coord) const
{
    return Rect.Left < coord.X && coord.X < Rect.Right && Rect.Top < coord.Y && coord.Y < Rect.Bottom;
}

void SViewport::OnResize(const FRect& NewRect)
{
    Rect = NewRect;
    if (!Viewport)
    {
        return;
    }

    int32 Width = Rect.Right - Rect.Left;
    int32 Height = Rect.Bottom - Rect.Top;
    Viewport->Init(0, 0, Width, Height);
}
