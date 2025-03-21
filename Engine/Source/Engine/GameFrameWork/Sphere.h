#pragma once

#include "Actor.h"

class ASphere : public AActor
{
	UCLASS(ASphere, AActor)

public:
	ASphere();
	virtual ~ASphere() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

