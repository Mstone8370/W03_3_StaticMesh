#pragma once
#include "MeshComponent.h"

class UStaticMeshComponent : public UMeshComponent
{
    UCLASS(UStaticMeshComponent, UMeshComponent)
    
public:
    UStaticMeshComponent() = default;
    
public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
};
