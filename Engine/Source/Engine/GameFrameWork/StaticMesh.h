#pragma once
#include "Object.h"

struct FStaticMesh;

class UStaticMesh : public UObject
{
    UCLASS(UStaticMesh, UObject)

public:

    void SetStaticMeshAsset(FStaticMesh* InStaticMesh);
    FString GetAssetPathFileName();
private:
    FStaticMesh* StaticMeshAsset;
};