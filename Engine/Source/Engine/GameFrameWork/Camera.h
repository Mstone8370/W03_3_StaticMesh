﻿#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Transform.h"
#include "Core/HAL/PlatformType.h"
#include "Actor.h"
#include "Delegates/Delegate.h"

namespace ECameraProjectionMode
{
    enum Type : uint8
    {
        Perspective,
        Orthographic
    };
}

DECLARE_DELEGATE_OneParam(OnCameraMovedSignature, const FTransform&)
DECLARE_DELEGATE_OneParam(OnCameraProjectionChangedSignature, const ACamera*);

class ACamera : public AActor
{
    UCLASS(ACamera, AActor)
    
public:
    ACamera();

    virtual void SetActorTransform(const FTransform& InTransform) override;

    void SetFieldOfView(float InFov);
    void SetFar(float InFarClip);
    void SetNear(float InNearClip);
    void SetProjectionMode(ECameraProjectionMode::Type InProjectionMode);
    
    float GetFieldOfView() const { return FieldOfView; }
    float GetNearClip() const { return NearClip; }
    float GetFarClip() const { return FarClip; }
    float GetMaxPitch() const { return MaxPitch; }
    ECameraProjectionMode::Type GetProjectionMode() const { return ProjectionMode; }

    FMatrix GetViewMatrix() const { return GetActorTransform().GetViewMatrix(); }

    OnCameraMovedSignature OnCameraMoved;
    OnCameraProjectionChangedSignature OnCameraProjectionChanged;
    
    
private:    
    float NearClip;
    
    float FarClip;
    
    float FieldOfView; // 화면각
    
    float MaxPitch;
    
    // 투영 타입 - Perspective, Orthographic
    ECameraProjectionMode::Type ProjectionMode;
};