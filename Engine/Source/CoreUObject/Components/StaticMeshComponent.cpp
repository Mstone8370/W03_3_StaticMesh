#include "pch.h"
#include "StaticMeshComponent.h"

void UStaticMeshComponent::BeginPlay()
{
    UMeshComponent::BeginPlay();
}

void UStaticMeshComponent::Tick(float DeltaTime)
{
    UMeshComponent::Tick(DeltaTime);
}
