#include "pch.h"
#include "SWindow.h"
#include "Input/PlayerInput.h"

SWindow::SWindow()
    : Rect({0, 0, 0, 0})
    , Viewport(std::make_unique<SViewport>())
{}

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
