#include "pch.h"
#include "AStaticMesh.h"
#include "StaticMesh.h"
#include "CoreUObject/Components/StaticMeshComponent.h"

AStaticMesh::AStaticMesh()
{
	bCanEverTick = true;
	
	UStaticMeshComponent* MeshComponent = AddComponent<UStaticMeshComponent>();
	MeshComponent->SetMeshName("TestMesh");

	RootComponent = MeshComponent;

	SetActorTransform(FTransform());
}

void AStaticMesh::BeginPlay()
{
	Super::BeginPlay();
}

void AStaticMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* AStaticMesh::GetTypeName()
{
	return "StaticMesh";
}