#pragma once

#include "SimpleJSON/Json.hpp"

using json::JSON;
#include "Components/PrimitiveComponent.h"

class UMeshComp : public UPrimitiveComponent {
    UCLASS(UMeshComp, UPrimitiveComponent);
public:
    // 기본 메시 컴포넌트 기능 정의 (예: 머티리얼, 가시성 등 공통 속성)
    UMeshComp()
    {
        //bCanBeRendered=true;
    }
    virtual void Serialize(bool bIsLoading, JSON Handle)
    {
    }
    virtual void Render(URenderer* Renderer) override
    {
        
    }

};
