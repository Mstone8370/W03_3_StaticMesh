#pragma once
#include "Object.h"
#include "CoreUObject/Components/SceneComponent.h"

struct FStaticMesh;

class UStaticMesh : public UObject
{
    UCLASS(UStaticMesh, UObject)

public:
    FStaticMesh* StaticMeshAsset;

    const FString& GetAssetPathFileName();
    void SetStaticMeshAsset(FStaticMesh* StaticMesh);
};