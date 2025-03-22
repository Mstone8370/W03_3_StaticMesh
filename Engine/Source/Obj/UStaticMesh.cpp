#include "pch.h"
#include "UStaticMesh.h"
// UStaticMesh.cpp

REGISTER_CLASS(UStaticMesh);
UStaticMesh::UStaticMesh()
{
}

const std::string& UStaticMesh::GetAssetPathFileName() const {
    return StaticMeshAsset->PathFileName;
}

void UStaticMesh::SetStaticMeshAsset(FStaticMesh* InStaticMesh) {
    StaticMeshAsset = InStaticMesh;
}
FStaticMesh* UStaticMesh::GetStaticMeshAsset()
{
    if (!StaticMeshAsset)return nullptr;
    return StaticMeshAsset;
}
