#include "pch.h"
#include "ObjManager.h"

#include "ObjectFactory.h"
#include "GameFrameWork/StaticMesh.h"

TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
    if(FStaticMesh** It = ObjStaticMeshMap.Find(PathFileName))
    {
        return *It;
    }
    
    // TODO: OBJ Parsing and create a new FStaticMesh
    FStaticMesh* NewStaticMesh = nullptr;
    ObjStaticMeshMap.Add(PathFileName, NewStaticMesh);
    return NewStaticMesh;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(const FString& PathFileName)
{
    // TODO: TObjectIterator로 순환하여 이미 존재하는 경우 바로 리턴
    /*
    for (TObjectIterator<UStaticMesh> It; It; ++It)
    {
        UStaticMesh* StaticMesh = *It;
        if (StaticMesh->GetAssetPathFileName() == PathFileName)
            return It;
    }
    */

    FStaticMesh* StaticMeshAsset = LoadObjStaticMeshAsset(PathFileName);
    UStaticMesh* StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
    StaticMesh->SetStaticMeshAsset(StaticMeshAsset);
    return StaticMesh;
}
