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
    bRenderBoundingBox = false;

    // 루트 컴포넌트 생성
    USceneComponent* Root = AddComponent<USceneComponent>();
    RootComponent = Root;

    // UStaticMeshComponent 생성 및 설정
    MeshComponent = AddComponent<UStaticMeshComponent>();
    MeshComponent->SetupAttachment(RootComponent);
    
    FString staticMeshPath = "Resources/" + DefaultMesh;
    UStaticMesh* StaticMesh = FObjManager::LoadObjStaticMesh(staticMeshPath);
    
    MeshComponent->SetStaticMesh(StaticMesh);
    MeshComponent->SetCanBeRendered(true);
    MeshComponent->SetUseVertexColor(false);
  
    UEngine::Get().GetWorld()->AddRenderComponent(MeshComponent);


    UStaticMesh* Mesh = MeshComponent->GetStaticMesh();
    if (Mesh)
    {
        FString ObjPath = Mesh->GetAssetPathFileName();
        OutputDebugString(ObjPath.c_wchar());
        OutputDebugString(L"\n");
        UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);
    }
    /*UStaticMesh* StaticMesh = FObjManager::LoadObjStaticMesh(TEXT("Resources/plant.obj"));
    
    MeshComponent->SetStaticMesh(StaticMesh);
    MeshComponent->SetCanBeRendered(true);
    MeshComponent->SetUseVertexColor(false);
  
    UEngine::Get().GetWorld()->AddRenderComponent(MeshComponent);

}

    Mesh = MeshComponent->GetStaticMesh();
    if (Mesh)
    {
        FString ObjPath = Mesh->GetAssetPathFileName();
        UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);
    }   */

}

void AStaticMesh::Tick(float DeltaTime)
{
}

const char* AStaticMesh::GetTypeName()
{
    return "AStaticMesh";
}

const FString AStaticMesh::GetDefaultMesh()
{
    return DefaultMesh;
}
