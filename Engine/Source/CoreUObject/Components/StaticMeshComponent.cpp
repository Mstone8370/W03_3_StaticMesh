#include "pch.h"
#include "StaticMeshComponent.h"
#include "Static/ObjManager.h"

void UStaticMeshComponent::BeginPlay()
{
    UMeshComponent::BeginPlay();
}

void UStaticMeshComponent::Tick(float DeltaTime)
{
    UMeshComponent::Tick(DeltaTime);
}

void UStaticMeshComponent::ChangeStaticMesh(FString changeMeshName)
{
    FString NewMeshPath = "Resources/" + changeMeshName;
    UStaticMesh* NewStaticMesh = FObjManager::LoadObjStaticMesh(NewMeshPath);

    SetStaticMesh(NewStaticMesh);

    UStaticMesh* NewMesh = GetStaticMesh();
    if (NewMesh)
    {
        FString NewObjPath = NewMesh->GetAssetPathFileName();
        UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(NewObjPath);
    }
}