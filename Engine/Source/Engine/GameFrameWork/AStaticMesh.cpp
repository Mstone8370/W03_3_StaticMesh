#include "pch.h"
#include "AStaticMesh.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "CoreUObject/World.h"
#include "Core/Rendering/URenderer.h"
#include "Static/ObjManager.h"
#include "Core/Rendering/BufferCache.h"

AStaticMesh::AStaticMesh()
{
    bCanEverTick = true;
    bUseBoundingBox = true;
    bRenderBoundingBox = true;

    // UStaticMeshComponent 생성 및 루트로 설정
    MeshComponent = AddComponent<UStaticMeshComponent>();
    RootComponent = MeshComponent;
    
    // Default Mesh 로드
    FString staticMeshPath = "Resources/" + AssetName;
    UStaticMesh* StaticMesh = FObjManager::LoadObjStaticMesh(staticMeshPath);
    MeshComponent->SetStaticMesh(StaticMesh);
    MeshComponent->SetCanBeRendered(true);
    MeshComponent->SetUseVertexColor(false);
    UEngine::Get().GetWorld()->AddRenderComponent(MeshComponent);

    UStaticMesh* Mesh = MeshComponent->GetStaticMesh();
    if (Mesh)
    {
        FString ObjPath = Mesh->GetAssetPathFileName();
        UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);
    }
}

void AStaticMesh::BeginPlay() {}

void AStaticMesh::Tick(float DeltaTime)
{
}

const char* AStaticMesh::GetTypeName()
{
    return "AStaticMesh";
}

void AStaticMesh::InitStaticMeshBoundingBox()
{
    MeshComponent->InitStaticMeshBoundingBox();
}
