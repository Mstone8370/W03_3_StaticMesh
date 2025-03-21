#pragma once
#include "MeshComponent.h"

class UStaticMeshComponent : public UMeshComponent
{
    UCLASS(UStaticMeshComponent, UMeshComponent)
    using Super = UMeshComponent;
    
public:
    UStaticMeshComponent() = default;
    
public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
};
