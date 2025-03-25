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
	virtual const FString GetAssetName() { return AssetName; }
	void SetAssetName(FString newAssetName ) { 
		AssetName = newAssetName;
		MeshComponent->ChangeStaticMesh(newAssetName);
	}
private:
	UStaticMeshComponent* MeshComponent;
	FString AssetName = "BT.obj";
};