#include "pch.h"
#include "Viewport.h"

#include "Engine.h"

FViewportClient::FViewportClient()
    : EditorCamera()
    , ViewMatrix(FMatrix::Identity)
    , ProjectionMatrix(FMatrix::Identity)
    , Width(0)
    , Height(0)
{
    
}

void FViewportClient::Draw()
{
    // TODO: 렌더러에 접근해서 실제 랜더 진행
    UEngine::Get().GetRenderer();
}

void FViewportClient::OnResize(int32 InWidth, int32 InHeight)
{
    Width = InWidth;
    Height = InHeight;
    // TODO: ProjectionMatrix 업데이트
}

void FViewportClient::HandleInput(const float DeltaTime)
{
    // TODO: 여기에서 카메라를 직접 조작해야함. ViewMatrix 업데이트
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

void FViewport::OnResize(int32 InWidth, int32 InHeight)
{
    if (ViewportClient)
    {
        ViewportClient->OnResize(InWidth, InHeight);
    }
}

void FViewport::HandleInput(const float DeltaTime)
{
    if (ViewportClient)
    {
        ViewportClient->HandleInput(DeltaTime);
    }
}
