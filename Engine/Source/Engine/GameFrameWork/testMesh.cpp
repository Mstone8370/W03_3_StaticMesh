#include "pch.h"
#include "testMesh.h"
#include "Engine/GameFrameWork/Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "CoreUObject/World.h"
#include "Core/Input/PlayerInput.h"
#include "Static/EditorManager.h"
#include "Components/StaticMeshComponent.h"

testMesh::testMesh() {
	bCanEverTick = true;
	bUseBoundingBox = true;
	bRenderBoundingBox = false;
	USceneComponent* Root = AddComponent<USceneComponent>();
	RootComponent = Root;

	UStaticMeshComponent* mesh = AddComponent<UStaticMeshComponent>();
	mesh->SetupAttachment(RootComponent);
	mesh->SetMeshName("cat");
	mesh->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	mesh->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), {1,1,1}));
	mesh->SetCanBeRendered(true);

	UEngine::Get().GetWorld()->AddRenderComponent(mesh);
}
void testMesh::BeginPlay()
{
	
}

void testMesh::Tick(float DeltaTime)
{
}

const char* testMesh::GetTypeName()
{
	return "testMesh";
}
