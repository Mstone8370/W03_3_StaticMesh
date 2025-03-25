#include "pch.h"
#include "StaticMesh.h"
#include "ObjManager.h"

void UStaticMesh::SetStaticMeshAsset(FStaticMesh* InStaticMesh)
{
    // TODO:
    StaticMeshAsset = InStaticMesh;
}

FString UStaticMesh::GetAssetPathFileName()
{
    return  (StaticMeshAsset) ? StaticMeshAsset->PathFileName : "";
}
