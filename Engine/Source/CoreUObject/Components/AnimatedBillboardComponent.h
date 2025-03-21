#pragma once
#include "BillboardComponent.h"

class UAnimatedBillboardComponent : public UBillboardComponent
{
    UCLASS(UAnimatedBillboardComponent, UBillboardComponent)
    
public:
    UAnimatedBillboardComponent();
    virtual ~UAnimatedBillboardComponent() = default;

    virtual void BeginPlay() override;
    
    virtual void Tick(float DeltaTime) override;

    virtual void Render(class URenderer* Renderer) override;

    void SetPlayRate(int32 InPlayRate) { PlayRate = InPlayRate; }

private:
    int32 PlayRate; // FPS

    float RemainingNextFrameTime;

    int32 UvIndex;
};
