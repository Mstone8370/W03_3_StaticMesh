#pragma once
#include "Object.h"

struct FStaticMesh;

class UStaticMesh : public UObject
{
    UCLASS(UStaticMesh, UObject)
        using Super = UStaticMesh;

public:

    void SetStaticMeshAsset(FStaticMesh* StaticMesh);
};