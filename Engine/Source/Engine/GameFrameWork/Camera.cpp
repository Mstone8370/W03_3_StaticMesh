#include "pch.h" 
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

    if (URenderer* Renderer = UEngine::Get().GetRenderer())
    {
        /**
         * 기존에는 외부에서 카메라의 트랜스폼을 변경할 때마다 추가적으로 렌더러의 UpdateViewMatrix를 호출해야함.
         * 따라서 ACamera에서 호출해주면 실수로 렌더러의 UpdateViewMatrix를 호출하지 못하는 경우를 방지할 수 있을 듯.
         */
        Renderer->UpdateViewMatrix(InTransform);
    }
}

void ACamera::SetFieldOfView(float InFov)
{
    FieldOfView = InFov;
    OnProjectionMatrixChanged();
}

void ACamera::SetFar(float InFarClip)
{
    FarClip = InFarClip;
    OnProjectionMatrixChanged();
}

void ACamera::SetNear(float InNearClip)
{
    NearClip = InNearClip;
    OnProjectionMatrixChanged();
}

void ACamera::SetProjectionMode(ECameraProjectionMode::Type InProjectionMode)
{
    ProjectionMode = InProjectionMode;
    OnProjectionMatrixChanged();
}

void ACamera::OnProjectionMatrixChanged() const
{
    if (URenderer* Renderer = UEngine::Get().GetRenderer())
    {
        Renderer->UpdateProjectionMatrix(this);
    }
}
