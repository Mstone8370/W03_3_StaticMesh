#pragma once
#include "MeshComponent.h"
#include "GameFramework/StaticMesh.h"

class UStaticMeshComponent : public UMeshComponent
{
    UCLASS(UStaticMeshComponent, UMeshComponent);
public:
    UStaticMeshComponent() = default;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    void InitStaticMeshBoundingBox(UWorld* World);
    UStaticMesh* GetStaticMesh() const { return StaticMesh; }
    void SetStaticMesh(UStaticMesh* InStaticMesh) { StaticMesh = InStaticMesh; }
    void ChangeStaticMesh(FString changeMeshPath);
private:
    UStaticMesh* StaticMesh;
};
