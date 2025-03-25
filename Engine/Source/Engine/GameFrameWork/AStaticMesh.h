#pragma once
#include "Actor.h"
#include "Components/StaticMeshComponent.h"

class AStaticMesh : public AActor
{
	UCLASS(AStaticMesh, AActor)

public:
	AStaticMesh();
	virtual ~AStaticMesh() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
	virtual const FString GetDefaultMesh();
private:
	UStaticMeshComponent* MeshComponent;
	FString DefaultMesh = "mst.obj";
};