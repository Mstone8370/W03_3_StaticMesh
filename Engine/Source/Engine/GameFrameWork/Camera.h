#pragma once

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
    
    float GetOrthoWidth() const { return OrthoWidth; }
    float GetOrthoHeight() const { return OrthoHeight; }
    void SetOrthoSize(float Width,float Height)
    {
        OrthoWidth = Width;
        //float Aspect = Width/Height; // or per-viewport ratio
        OrthoHeight = Height;
    }
    void SetOrthoSize(float Width,FRect ViewportRect)
    {
        OrthoWidth = Width;
        float Aspect = (ViewportRect.Height > 0.f)
            ? (ViewportRect.Width / ViewportRect.Height)
            : 1.0f; // fallback
        OrthoHeight = Width/Aspect;
    }
    FMatrix GetProjectionMatrix(float AspectRatio) const;

private:    
    float NearClip;
    
    float FarClip;
    
    float FieldOfView; // 화면각
    
    float MaxPitch;
    float OrthoWidth = 5.0f;  // 기본값
    float OrthoHeight = 5.0f;
    // 투영 타입 - Perspective, Orthographic
    ECameraProjectionMode::Type ProjectionMode;
};