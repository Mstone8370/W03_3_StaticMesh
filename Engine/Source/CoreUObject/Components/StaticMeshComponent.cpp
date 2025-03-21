#include "pch.h"
#include "StaticMeshComponent.h"

REGISTER_CLASS(UStaticMeshComponent)

void UStaticMeshComponent::BeginPlay()
{
    UMeshComponent::BeginPlay();
}

void UStaticMeshComponent::Tick(float DeltaTime)
{
    UMeshComponent::Tick(DeltaTime);
}
