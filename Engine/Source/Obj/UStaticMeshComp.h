// UStaticMeshComp.h
#pragma once
#include "UMeshComp.h"
#include "UStaticMesh.h"
class UStaticMeshComp : public UMeshComp {
    UCLASS(UStaticMeshComp, UMeshComp);
private:
    UStaticMesh* StaticMesh;
    
public:
    UStaticMeshComp();
    void SetStaticMesh(UStaticMesh* InMesh) { StaticMesh = InMesh; }
    UStaticMesh* GetStaticMesh() const { if (!StaticMesh)return nullptr; return  StaticMesh; }

    virtual void Render(URenderer* Renderer) override;
    virtual void Serialize(bool bIsLoading, JSON Handle) override;
};
