#pragma once
#include "Object.h"

struct FStaticMesh;

class UStaticMesh : public UObject
{
    UCLASS(UStaticMesh, UObject)

public:

    void SetStaticMeshAsset(FStaticMesh* StaticMesh);
};