#pragma once
#include "MeshComponent.h"
#include "GameFramework/StaticMesh.h"
class UStaticMeshComponent : public UMeshComponent
{
    UCLASS(UStaticMeshComponent, UMeshComponent)
    
public:
    UStaticMeshComponent() = default;
    UStaticMesh* StaticMesh;
public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
};
