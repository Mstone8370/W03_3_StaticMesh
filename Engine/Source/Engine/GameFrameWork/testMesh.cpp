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
    
    MeshComponent->SetMeshName("x-35_obj"); // Mesh 이름 (키)
    UE_LOG(MeshComponent->GetMeshName().ToString().c_char());

    // OBJ 파일로부터 UStaticMesh 생성 및 연결 (경로 예시)
    UStaticMesh* StaticMesh = FObjManager::LoadObjStaticMesh(TEXT("Resources/x-35_obj.obj"));
    MeshComponent->SetStaticMesh(StaticMesh);
    MeshComponent->SetCanBeRendered(true);
    MeshComponent->SetUseVertexColor(true);
    // 렌더 컴포넌트로 등록 (예: 월드에 추가)
    UEngine::Get().GetWorld()->AddRenderComponent(MeshComponent);
}

void testMesh::BeginPlay()
{
    // 액터 시작 시, 버퍼 캐시에 UStaticMesh의 버퍼를 등록
    UStaticMesh* Mesh = MeshComponent->GetStaticMesh();
    if (Mesh)
    {
        FString ObjPath = Mesh->GetAssetPathFileName();
        UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);
    }
}

void testMesh::Tick(float DeltaTime)
{
}

const char* testMesh::GetTypeName()
{
    return "testMesh";
}
