#pragma once

#include "Engine/GameFrameWork/Actor.h"

class UStaticMeshComponent;

enum class ESelectedAxis : uint8
{
	None,
	X,
	Y,
	Z
};

enum class EGizmoType : uint8
{
	Translate,
	Rotate,
	Scale,
	Max
};

class AGizmoHandle : public AActor
{
	UCLASS(AGizmoHandle, AActor)

public:
	AGizmoHandle();

public:
	virtual void Tick(float DeltaTime) override;
	void SetScaleByDistance();
	void SetActive(bool bActive);
	void SetSelectedAxis(ESelectedAxis NewAxis) { SelectedAxis = NewAxis; }
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }
	EGizmoType GetGizmoType() const { return GizmoType; }
	void SetLocal(bool bIsLocal) { this->bIsLocal = bIsLocal; }

private:
	void InitTranslationGizmo();
	void InitRotationGizmo();
	void InitScaleGizmo();

	void OnGizmoTypeChanged(EGizmoType NewGizmoType);

	void HideAllGizmo();

	FVector GizmoScale = FVector(0.5f, 0.5f, 0.5f);
	
	bool bIsActive = false;
	TArray<UStaticMeshComponent*> AllGizmos;

	TArray<UStaticMeshComponent*> TranslationGizmos;
	TArray<UStaticMeshComponent*> RotationGizmos;
	TArray<UStaticMeshComponent*> ScaleGizmos;

	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	
	EGizmoType GizmoType = EGizmoType::Translate;

public:
	virtual const char* GetTypeName() override;

	bool bIsLocal = false;
	
private:
	void DoTransform(FTransform& CompTransform, FVector Result, USceneComponent* SceneComp);

	FVector CachedRayResult = FVector::ZeroVector;
};


