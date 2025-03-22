#include "pch.h"
#include "StaticMesh.h"
#include "ObjManager.h"

const FString& UStaticMesh::GetAssetPathFileName()
{
    return StaticMeshAsset->PathFileName;
}

void UStaticMesh::SetStaticMeshAsset(FStaticMesh* InStaticMesh)
{
    StaticMeshAsset = InStaticMesh;
}
