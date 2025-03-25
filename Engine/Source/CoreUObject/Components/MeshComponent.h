#pragma once
#include "PrimitiveComponent.h"

class UMeshComponent : public UPrimitiveComponent
{
    UCLASS(UMeshComponent, UPrimitiveComponent);
    
public:
    UMeshComponent() = default;
    
public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Render(URenderer* Renderer) override;
    
   
   /*
   * (Deprecated)
   * 과거에는 BufferCache에서 FName(MeshName) 기반으로 키 참조에 사용했으나,
   * 현재는 파일 경로 기반으로 직접 관리되므로 더 이상 사용되지 않음.
   */
    void SetMeshName(FName InMeshName) { MeshName = InMeshName; }
    FName GetMeshName() const { return MeshName; }
    
private:
    FName MeshName;
    
};
