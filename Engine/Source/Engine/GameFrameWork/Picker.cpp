#include "pch.h"
#include "Picker.h"
#include "Core/HAL/PlatformType.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Rendering/URenderer.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Gizmo/GizmoHandle.h"
#include "Static/EditorManager.h"
#include "Camera.h"
#include "Core/Math/Ray.h"
#include "World.h"
#include "Editor/Viewport/FViewport.h"
#include "Input/PlayerController.h"
#include "Static/EditorManager.h"

APicker::APicker()
{
    bIsGizmo = true;
}

FVector4 APicker::EncodeUUID(unsigned int UUID)
{
    float a = (UUID >> 24) & 0xff;
    float b = (UUID >> 16) & 0xff;
    float g = (UUID >> 8) & 0xff;
    float r = UUID & 0xff;

    FVector4 color = {r, g, b, a};

    return color;
}

int APicker::DecodeUUID(FVector4 color)
{
    return (static_cast<unsigned int>(color.W) << 24) | (static_cast<unsigned int>(color.Z) << 16) | (static_cast<
        unsigned int>(color.Y) << 8) | (static_cast<unsigned int>(color.X));
}

void APicker::BeginPlay()
{
}

void APicker::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APicker::LateTick(float DeltaTime)
{
    AActor::LateTick(DeltaTime);

    if (APlayerController::Get().IsUiInput())
    {
        return;
    }

    if (APlayerInput::Get().IsMousePressed(false)) //좌클릭
    {
        if (!PickByColor())
        {
            if (!PickByRay())
            {
                FEditorManager::Get().ClearSelectedComponent();
            }
        }
    }


    // 기즈모 핸들링
    if (APlayerInput::Get().IsMouseDown(false)) //좌클릭
    {
        HandleGizmo();
    }
    else
    {
        if (AGizmoHandle* Handle = FEditorManager::Get().GetGizmoHandle())
        {
            Handle->SetSelectedAxis(ESelectedAxis::None);
        }
    }
}

const char* APicker::GetTypeName()
{
    return "Picker";
}

bool APicker::PickByColor()
{
    int32 X, Y;
    APlayerInput::Get().GetMousePositionClient(X, Y);

    int32 ViewportIndex = APlayerController::Get().GetClickedViewportIndex();
    if (ViewportIndex == -1)
        return false;

    URenderer* Renderer = UEngine::Get().GetRenderer();
    SViewport* SView = Renderer->Viewports[ViewportIndex];
    if (!SView) return false;

    FViewport* FView = SView->GetFViewport();
    if (!FView) return false;

    FVector4 Color = FView->GetPickingPixel(Renderer->GetDeviceContext(), X, Y);
    uint32 UUID = DecodeUUID(Color);

    USceneComponent* PickedComponent = UEngine::Get().GetObjectByUUID<USceneComponent>(UUID);

    if (PickedComponent)
    {
        if (!PickedComponent->GetOwner()->IsGizmoActor())
            FEditorManager::Get().SelectComponent(PickedComponent);

        UE_LOG("Pick - UUID: %d", UUID);
        return true;
    }

    return false;
}



bool APicker::PickByRay()
{
    int32 ViewportIndex = APlayerController::Get().GetClickedViewportIndex();
    if (ViewportIndex == -1)
        return false;

    URenderer* Renderer = UEngine::Get().GetRenderer();
    SViewport* SView = Renderer->Viewports[ViewportIndex];
    if (!SView) return false;

    FViewport* FView = SView->GetFViewport();
    if (!FView || !FView->GetCamera())
        return false;
    const FRect& Rect = SView->GetRect();
    //Renderer->UpdateViewMatrix(FView->GetCamera()->GetActorTransform());
    //Renderer->UpdateProjectionMatrixAspect(FView->GetCamera(), Rect.Width, Rect.Height);

    FRay Ray = FRay::GetRayByViewportPoint(FView->GetCamera(), Rect);

    USceneComponent* FirstHitComponent = nullptr;
    if (GetWorld()->LineTrace(Ray, FirstHitComponent))
    {
        if (FirstHitComponent)
        {
            FEditorManager::Get().SelectComponent(FirstHitComponent);
            return true;
        }
    }

    return false;
}

void APicker::HandleGizmo()
{
    int32 X = 0, Y = 0;
    APlayerInput::Get().GetMousePositionClient(X, Y);

    int32 ViewportIndex = APlayerController::Get().GetClickedViewportIndex();
    if (ViewportIndex == -1)
        return;

    URenderer* Renderer = UEngine::Get().GetRenderer();
    if (ViewportIndex >= Renderer->Viewports.Num())
        return;

    SViewport* SView = Renderer->Viewports[ViewportIndex];
    FViewport* FView = SView->GetFViewport();
    if (!FView)
        return;

    FVector4 Color = FView->GetPickingPixel(Renderer->GetDeviceContext(), X, Y);
    uint32 UUID = DecodeUUID(Color);

    UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);
    if (!PickedComponent) return;

    if (AActor* Owner = PickedComponent->GetOwner())
    {
        if (Owner->IsA<AGizmoHandle>())
        {
            AGizmoHandle* Gizmo = static_cast<AGizmoHandle*>(Owner);
            if (Gizmo->GetSelectedAxis() != ESelectedAxis::None)
                return;

            UCylinderComp* Cylinder = static_cast<UCylinderComp*>(PickedComponent);
            FVector4 CompColor = Cylinder->GetCustomColor();

            if (1.0f - FMath::Abs(CompColor.X) < KINDA_SMALL_NUMBER)
                Gizmo->SetSelectedAxis(ESelectedAxis::X);
            else if (1.0f - FMath::Abs(CompColor.Y) < KINDA_SMALL_NUMBER)
                Gizmo->SetSelectedAxis(ESelectedAxis::Y);
            else
                Gizmo->SetSelectedAxis(ESelectedAxis::Z);
        }
    }
}
