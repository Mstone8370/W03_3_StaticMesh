#pragma once

#include "Actor.h"

class AArrow : public AActor
{
	UCLASS(AArrow, AActor)

public:
	AArrow();
	virtual ~AArrow() = default;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

