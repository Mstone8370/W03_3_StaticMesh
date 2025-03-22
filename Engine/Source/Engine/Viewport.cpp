#include "pch.h"
#include "Viewport.h"

#include "Engine.h"

FViewportClient::FViewportClient()
    : Camera(nullptr)
{
}

void FViewportClient::Draw()
{
    // TODO: 렌더러에 접근해서 실제 랜더 진행
    UEngine::Get().GetRenderer();
}

FViewport::FViewport()
    : ViewportClient(std::make_unique<FViewportClient>())
{}

void FViewport::Draw()
{
    if (ViewportClient)
    {
        ViewportClient->Draw();
    }
}
