﻿#include "pch.h" 
#include "Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"

ACamera::ACamera()
    : NearClip(0.1f)
    , FarClip(100.f)
    , FieldOfView(90.f)
    , ProjectionMode(ECameraProjectionMode::Perspective)
    , MaxPitch(89.8f)
{
    bIsGizmo = true;

    RootComponent = AddComponent<USceneComponent>();

    FTransform StartTransform = GetActorTransform();
    FVector StartLocation(3.f, -2.f, 2.f);
    StartTransform.Translate(StartLocation);
	StartTransform.LookAt(FVector::ZeroVector);
    SetActorTransform(StartTransform);
}

void ACamera::SetActorTransform(const FTransform& InTransform)
{
    AActor::SetActorTransform(InTransform);

    // 위치 업데이트 알림
    OnCameraMoved.Broadcast(InTransform);
}

void ACamera::SetFieldOfView(float InFov)
{
    FieldOfView = InFov;
    OnCameraProjectionChanged.Broadcast(this);
}

void ACamera::SetFar(float InFarClip)
{
    FarClip = InFarClip;
    OnCameraProjectionChanged.Broadcast(this);
}

void ACamera::SetNear(float InNearClip)
{
    NearClip = InNearClip;
    OnCameraProjectionChanged.Broadcast(this);
}

void ACamera::SetProjectionMode(ECameraProjectionMode::Type InProjectionMode)
{
    ProjectionMode = InProjectionMode;
    OnCameraProjectionChanged.Broadcast(this);
}
FMatrix ACamera::GetProjectionMatrix(float AspectRatio) const
{
    if (ProjectionMode == ECameraProjectionMode::Perspective)
    {
        float FOVRad = FMath::DegreesToRadians(FieldOfView);
        return FMatrix::PerspectiveFovLH(FOVRad, AspectRatio, NearClip, FarClip);
    }
    else // Orthographic
    {
        return FMatrix::OrthoLH(OrthoWidth, OrthoHeight, NearClip, FarClip);
    }
}
