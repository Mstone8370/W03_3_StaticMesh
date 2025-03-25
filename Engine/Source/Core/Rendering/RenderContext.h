#pragma once

class URenderer;
class UWorld;

struct FRenderContext
{
    URenderer* Renderer = nullptr;
    UWorld* World = nullptr;
    float DeltaTime = 0.f;
};
