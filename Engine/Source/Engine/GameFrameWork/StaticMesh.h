#pragma once
#include "Object.h"

struct FStaticMesh;

class UStaticMesh : public UObject
{
    UCLASS(UStaticMesh, UObject)

public:
    void SetStaticMeshAsset(FStaticMesh* InStaticMesh);
    FString GetAssetPathFileName();
    FStaticMesh* GetStaticMeshAsset() const {
        return StaticMeshAsset;
    }
private:
    FStaticMesh* StaticMeshAsset;
};