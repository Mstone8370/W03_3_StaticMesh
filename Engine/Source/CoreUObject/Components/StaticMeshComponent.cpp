#include "pch.h"
#include "StaticMeshComponent.h"
#include "CoreUObject/World.h"
#include "Engine/GameFrameWork/Actor.h"
#include "Static/ObjManager.h"

void UStaticMeshComponent::BeginPlay()
{
    UMeshComponent::BeginPlay();
}

void UStaticMeshComponent::Tick(float DeltaTime)
{
    UMeshComponent::Tick(DeltaTime);
}

void UStaticMeshComponent::InitStaticMeshBoundingBox(UWorld* World)
{
    FVector Min;
    FVector Max;
    FName meshKey = FObjImporter::GetNormalizedMeshKey(StaticMesh->GetAssetPathFileName());
    UEngine::Get().GetRenderer()->GetStaticMeshLocalBounds(meshKey, Min, Max);

    BoundingBox = std::make_shared<FBox>();
    BoundingBox->Init(this, Min, Max);
    if(World)
        World->AddBoundingBox(BoundingBox.get());

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
    InitStaticMeshBoundingBox(GetOwner()->GetWorld());
    UpdateBoundingBox();
}