#include "pch.h"
#include "UStaticMeshComp.h"

#include "FObjManager.h"

REGISTER_CLASS(UStaticMeshComp);
UStaticMeshComp::UStaticMeshComp()
{
}
void UStaticMeshComp::Serialize(bool bIsLoading, JSON Handle)
{
    //Super::Serialize(bIsLoading, Handle);

    if (bIsLoading)
    {
        FString assetName;
        //StaticMesh = FObjManager::LoadObjStaticMesh(assetName);
    }
    else
    {
        FString assetName = StaticMesh ? StaticMesh->GetAssetPathFileName() : "";
    }
}
void UStaticMeshComp::Render(URenderer* Renderer)
{
    if (!Renderer || !StaticMesh)
        return;
    if (!StaticMesh->GetStaticMeshAsset())return;
    // FStaticMesh 정보를 렌더러로 전달
    Renderer->RenderMesh(this);  // 핵심: RenderPrimitive가 아니라 RenderMesh
}
