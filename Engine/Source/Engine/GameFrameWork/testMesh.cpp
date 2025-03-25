#include "pch.h"
#include "testMesh.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "CoreUObject/World.h"
#include "Core/Rendering/URenderer.h"
#include "Static/ObjManager.h"
#include "Core/Rendering/BufferCache.h"

testMesh::testMesh()
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
    
    //UStaticMesh* StaticMesh = FObjManager::LoadObjStaticMesh(TEXT("Resources/mst.obj"));
    //UStaticMesh* StaticMesh = FObjManager::LoadObjStaticMesh(TEXT("Resources/cat.obj"));
    
    UStaticMesh* StaticMesh1 = FObjManager::LoadObjStaticMesh(TEXT("Resources/mst.obj"));
    
    UStaticMeshComponent* meshcomp = AddComponent<UStaticMeshComponent>();
    meshcomp->SetStaticMesh(StaticMesh1);
    meshcomp->SetCanBeRendered(true);
    meshcomp->SetUseVertexColor(false);
  
    UEngine::Get().GetWorld()->AddRenderComponent(meshcomp);


    UStaticMesh* Mesh = meshcomp->GetStaticMesh();
    if (Mesh)
    {
        FString ObjPath = Mesh->GetAssetPathFileName();
        OutputDebugString(ObjPath.c_wchar());
        OutputDebugString(L"\n");
        UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);
    }
    UStaticMesh* StaticMesh = FObjManager::LoadObjStaticMesh(TEXT("Resources/plant.obj"));
    
    MeshComponent->SetStaticMesh(StaticMesh);
    MeshComponent->SetCanBeRendered(true);
    MeshComponent->SetUseVertexColor(false);
  
    UEngine::Get().GetWorld()->AddRenderComponent(MeshComponent);


    Mesh = MeshComponent->GetStaticMesh();
    if (Mesh)
    {
        FString ObjPath = Mesh->GetAssetPathFileName();
        UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);
    }   

}

void testMesh::BeginPlay()
{
   
}

void testMesh::Tick(float DeltaTime)
{
}

const char* testMesh::GetTypeName()
{
    return "testMesh";
}
