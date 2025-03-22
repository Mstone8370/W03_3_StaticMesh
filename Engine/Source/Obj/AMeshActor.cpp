#include "pch.h"
#include "AMeshActor.h"

#include "FObjManager.h"
#include "UStaticMeshComp.h"

REGISTER_CLASS(AMeshActor);
void AMeshActor::BeginPlay()
{
    // 1. 컴포넌트 생성
    MeshComp = AddComponent<UStaticMeshComp>();

    // 2. 메시 로드: 캐싱되어 있으면 기존 사용
    UStaticMesh* Mesh = FObjManager::LoadObjStaticMesh("Resources/GizmoTranslation.obj");

    // 3. 메시 세팅
    MeshComp->SetStaticMesh(Mesh);

    // 4. 액터에 컴포넌트 부착
    MeshComp->bCanBeRendered=true;
    
    AActor::BeginPlay();
}

void AMeshActor::Destroyed()
{
    // 필요시 MeshComp 삭제 (엔진이 관리해주면 생략 가능)
    delete MeshComp;
}
void AMeshActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);

}
