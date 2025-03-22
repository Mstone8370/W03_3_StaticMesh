#pragma once
#include "GameFrameWork/Actor.h"

class UStaticMesh;
class UStaticMeshComp;

class AMeshActor:public AActor
{
public:
private:
    UStaticMeshComp* MeshComp;

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    //virtual void LateTick (float DeltaTime) override;
	
    virtual void Destroyed() override;
};
