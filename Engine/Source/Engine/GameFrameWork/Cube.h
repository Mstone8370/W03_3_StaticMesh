#pragma once

#include "Actor.h"

class ACube : public AActor
{
	UCLASS(ACube, AActor)

public:
	ACube();
	virtual ~ACube() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

