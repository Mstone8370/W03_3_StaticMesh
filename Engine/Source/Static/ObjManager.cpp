#include "pch.h"
#include "ObjManager.h"

#include "ObjectFactory.h"
#include "GameFrameWork/StaticMesh.h"
#include "Utils/FObjImporter.h"
#include "ObjectIterator.h"

// Cache
TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;

// OBJ 파일 -> FStaticMesh(Cooked Data)
FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
    if(FStaticMesh** It = ObjStaticMeshMap.Find(PathFileName))
    {
        return *It;
    }
    
    FStaticMesh* NewStaticMesh = new FStaticMesh();
    if (!FObjImporter::LoadStaticMesh(PathFileName, *NewStaticMesh))
    {
        delete NewStaticMesh;
        NewStaticMesh = nullptr;
    }
    else
        ObjStaticMeshMap.Add(PathFileName, NewStaticMesh);

    return NewStaticMesh;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(const FString& PathFileName)
{
    // 씬에 이미 존재할 경우 이를 반환
    for (TObjectIterator<UStaticMesh> It; It; ++It)
    {
        UStaticMesh* StaticMesh = *It;
        if (StaticMesh->GetAssetPathFileName() == PathFileName)
            return StaticMesh;
    }
    
    // 없을 경우 새로 로드
    FStaticMesh* StaticMeshAsset = LoadObjStaticMeshAsset(PathFileName);
    UStaticMesh* StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
    StaticMesh->SetStaticMeshAsset(StaticMeshAsset);
    return StaticMesh;
}
