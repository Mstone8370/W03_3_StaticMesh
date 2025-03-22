#include "pch.h"
#include "FObjManager.h"
#include "UStaticMesh.h"

REGISTER_CLASS(FObjManager);
std::map<std::string, FStaticMesh*> FObjManager::ObjStaticMeshMap;

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const std::string& PathFileName)
{
    auto it = ObjStaticMeshMap.find(PathFileName);
    if (it != ObjStaticMeshMap.end()) {
        return it->second;
    }

    FObjInfo Info;
    if (!FObjImporter::LoadObjFile(PathFileName, Info)) return nullptr;

    FStaticMesh* NewMesh = FObjImporter::ConvertToStaticMesh(Info);
    ObjStaticMeshMap[PathFileName] = NewMesh;
    return NewMesh;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(const std::string& PathFileName)
{
    // Unreal-style TObjectIterator 대체는 사용 환경에 따라 구현 필요
    // 여기서는 새로 생성하여 반환하는 방식으로 처리

    FStaticMesh* Asset = LoadObjStaticMeshAsset(PathFileName);
    if (!Asset) return nullptr;

    UStaticMesh* StaticMesh = new UStaticMesh();
    StaticMesh->SetStaticMeshAsset(Asset);
    return StaticMesh;
}
