#pragma once
#include "Container/Array.h"
#include "Container/Map.h"
#include "Container/String.h"

class UStaticMesh;
struct FNormalVertex;

class FObjManager
{
public:
    static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);

    static UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);

private:
    static TMap<FString, FStaticMesh*> ObjStaticMeshMap;
};