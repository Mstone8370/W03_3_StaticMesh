#pragma once
#include "Object.h"
#include "Rendering/RendererDefine.h"

class UStaticMesh : public UObject
{
    UCLASS(UStaticMesh, UObject)

public:
    FStaticMesh* StaticMeshAsset;
    const FString& GetAssetPathFileName()
    {
        return StaticMeshAsset->PathFileName;
    }
    void SetStaticMeshAsset(FStaticMesh* InStaticMesh);
};