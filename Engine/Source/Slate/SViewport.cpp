#include "pch.h"
#include "SViewport.h"

SViewport::SViewport()
    : Viewport(std::make_unique<FViewport>())
{}

void SViewport::Paint()
{
    if (Viewport)
    {
        Viewport->Draw();
    }
}
