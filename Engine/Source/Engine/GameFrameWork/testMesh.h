#pragma once
#include "Actor.h"
#include "Components/StaticMeshComponent.h"

class testMesh : public AActor
{
	UCLASS(testMesh, AActor)

public:
	testMesh();
	virtual ~testMesh() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
private:
	UStaticMeshComponent* MeshComponent;
};