#pragma once

#include "Actor.h"

class AStaticMesh : public AActor
{
	UCLASS(AStaticMesh, AActor)
public:
	AStaticMesh();
	virtual ~AStaticMesh() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};