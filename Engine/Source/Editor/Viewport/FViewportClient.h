#pragma once
struct FRenderContext;
class FViewport;
class URenderer;
class UWorld;
class FViewportClient
{
public:
    virtual ~FViewportClient() = default;

    virtual void Draw(FViewport* Viewport, const FRenderContext& Context);
    virtual void ProcessInput(FViewport* Viewport, float DeltaTime);

    void SetWorld(UWorld* InWorld) { World = InWorld; }

protected:
    UWorld* World = nullptr;
};