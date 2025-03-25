#include "pch.h" 
#include "PlayerController.h"

#include "Static/EditorManager.h"
#include "PlayerInput.h"
#include "Core/Math/Plane.h"
#include "Editor/Viewport/FViewport.h"
#include "Engine/Engine.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Engine/EngineConfig.h"
#include "Rendering/URenderer.h"

APlayerController::APlayerController()
    : CurrentSpeed(3.f)
    , MaxSpeed(10.f)
    , MinSpeed(1.0f)
    , MouseSensitivity(0.2f)
    , MaxSensitivity(0.5f)
    , MinSensitivity(0.05f)
{}

APlayerController::~APlayerController()
{
}

void APlayerController::HandleCameraMovement(float DeltaTime)
{
	if (bUiCaptured)
		return;

	if (!APlayerInput::Get().IsMouseDown(true))
	{
		if (APlayerInput::Get().IsMouseReleased(true)){}
			//ShowCursor(true);
		return;
	}

	int32 ViewportIndex = GetClickedViewportIndex();
	if (ViewportIndex == -1)
		return;

	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (!Renderer || ViewportIndex >= Renderer->Viewports.Num())
		return;

	ACamera* Camera = Renderer->Viewports[ViewportIndex]->GetFViewport()->GetCamera();
	if (!Camera)
		return;

	FTransform CameraTransform = Camera->GetActorTransform();
	const bool bIsPerspective = Camera->GetProjectionMode() == ECameraProjectionMode::Perspective;

	// 1. 마우스 회전 또는 패닝
	HandleCameraRotation(Camera, CameraTransform, bIsPerspective);

	// 2. 커서 고정
	if (APlayerInput::Get().IsMousePressed(true))
	{
		APlayerInput::Get().CacheCursorPosition();
		//ShowCursor(false);
	}
	APlayerInput::Get().FixMouseCursor();

	// 3. 이동 입력 처리 (WASDQE)
	FVector MoveDir = GetCameraMovementDirection(Camera, bIsPerspective);

	if (MoveDir.Length() > 0.001f)
	{
		MoveDir = MoveDir.GetSafeNormal();
		CameraTransform.Translate(MoveDir * DeltaTime * CurrentSpeed);
	}

	Camera->SetActorTransform(CameraTransform);
	SaveCameraProperties(Camera);
}

void APlayerController::HandleCameraRotation(ACamera* Camera, FTransform& Transform, bool bIsPerspective)
{
	int32 DeltaX = 0, DeltaY = 0;
	APlayerInput::Get().GetMouseDelta(DeltaX, DeltaY);

	if (bIsPerspective)
	{
		FVector Euler = Transform.GetRotation().GetEuler();
		Euler.Y += MouseSensitivity * static_cast<float>(DeltaY); // Pitch
		Euler.Z += MouseSensitivity * static_cast<float>(DeltaX); // Yaw
		Euler.Y = FMath::Clamp(Euler.Y, -Camera->GetMaxPitch(), Camera->GetMaxPitch());
		Transform.SetRotation(Euler);
	}
	else
	{
		FVector Right = Camera->GetActorRightVector();
		FVector Up = Camera->GetActorUpVector();
		FVector PanOffset = (-Right * DeltaX + Up * DeltaY) * 0.01f;
		Transform.Translate(PanOffset);
	}
}
FVector APlayerController::GetCameraMovementDirection(ACamera* Camera, bool bIsPerspective)
{
	FVector Dir = FVector::Zero();

	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::A))
		Dir -= Camera->GetActorRightVector();
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::D))
		Dir += Camera->GetActorRightVector();

	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::W))
		Dir += bIsPerspective ? Camera->GetActorForwardVector() : Camera->GetActorUpVector();
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::S))
		Dir -= bIsPerspective ? Camera->GetActorForwardVector() : Camera->GetActorUpVector();

	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::Q))
		Dir -= FVector(0, 0, 1);
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::E))
		Dir += FVector(0, 0, 1);

	return Dir;
}



void APlayerController::SaveCameraProperties(ACamera* Camera)
{
    FEngineConfig* EngineConfig = UEngine::Get().GetEngineConfig();
    FTransform CameraTransform = Camera->GetActorTransform();

    float FOV = Camera->GetFieldOfView();
    float NearClip = Camera->GetNearClip();
    float FarClip = Camera->GetFarClip();
    float CameraSpeed = CurrentSpeed;

    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosX, CameraTransform.GetPosition().X);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosY, CameraTransform.GetPosition().Y);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosZ, CameraTransform.GetPosition().Z);

    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotX, CameraTransform.GetRotation().X);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotY, CameraTransform.GetRotation().Y);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotZ, CameraTransform.GetRotation().Z);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotW, CameraTransform.GetRotation().W);
}

void APlayerController::SetCurrentSpeed(float InSpeed)
{
    CurrentSpeed = FMath::Clamp(InSpeed, MinSpeed, MaxSpeed);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraSpeed, CurrentSpeed);
}

void APlayerController::SetMouseSensitivity(float InSensitivity)
{
    MouseSensitivity = FMath::Clamp(InSensitivity, MinSensitivity, MaxSensitivity);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraSensitivity, MouseSensitivity);
}

void APlayerController::ProcessPlayerInput(float DeltaTime)
{
    if (bUiInput)
    {
        if (APlayerInput::Get().IsMouseDown(true) || APlayerInput::Get().IsMouseDown(false))
        {
            bUiCaptured = true;
        }
        return;
    }
    if (bUiCaptured)
    {
        if (!APlayerInput::Get().IsMouseDown(true) && !APlayerInput::Get().IsMouseDown(false))
        {
            bUiCaptured = false;
        }
        return;
    }
	UpdateViewportClickState();
    // TODO: 기즈모 조작시에는 카메라 입력 무시
    // HandleGizmoMovement(DeltaTime); // TODO: 의미없는 함수인듯
    //HandleCameraMovement(DeltaTime);
}
bool APlayerController::HandleViewportDrag(float ViewportWidth, float ViewportHeight)
{
    bool result = false;
    int32 MouseX, MouseY;
    APlayerInput::Get().GetMousePositionClient(MouseX, MouseY);
    URenderer *Renderer = UEngine::Get().GetRenderer();
    float DragX = ViewportWidth * Renderer->HorizontalSplitRatio;
    float DragY = ViewportHeight * Renderer->VerticalSplitRatio;

    bool bHoverHorizontal = abs(MouseX - DragX) <= DragHandleSize;
    bool bHoverVertical = abs(MouseY - DragY) <= DragHandleSize;

    if (APlayerInput::Get().IsMousePressed(false))
    {
        if (bHoverHorizontal) bDraggingHorizontal = true;
        if (bHoverVertical) bDraggingVertical = true;
    }
    else if (!APlayerInput::Get().IsMouseDown(false))
    {
        bDraggingHorizontal = false;
        bDraggingVertical = false;
    }

    if (bDraggingHorizontal)
    {
        Renderer->HorizontalSplitRatio = FMath::Clamp(static_cast<float>(MouseX) / ViewportWidth, 0.1f, 0.9f);
        result = true;
    }
    if (bDraggingVertical)
    {
        Renderer->VerticalSplitRatio = FMath::Clamp(static_cast<float>(MouseY) / ViewportHeight, 0.1f, 0.9f);
        result = true;
    }
    return result;
}
int32 APlayerController::GetClickedViewportIndex()
{
	int32 MouseX, MouseY;
	APlayerInput::Get().GetMousePositionClient(MouseX, MouseY);

	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (!Renderer) return -1;

	const TArray<SViewport*>& Viewports = Renderer->Viewports;
	FPoint MousePos(static_cast<float>(MouseX), static_cast<float>(MouseY));

	for (int32 i = 0; i < Viewports.Num(); ++i)
	{
		SViewport* SView = Viewports[i];
		if (!SView) continue;

		const FRect& Rect = SView->GetRect();

		if (Rect.Contains(MousePos))
		{
			return i;
		}
	}

	return -1; // 어떤 뷰포트도 클릭되지 않음
}
void APlayerController::UpdateViewportClickState()
{
	if (APlayerInput::Get().IsMousePressed(true) || APlayerInput::Get().IsMousePressed(false)) // 우클릭 or 좌클릭
	{
		ClickedViewportIndex = GetClickedViewportIndex();
	}

	if (APlayerInput::Get().IsMouseReleased(true) || APlayerInput::Get().IsMouseReleased(false))
	{
		ClickedViewportIndex = -1;
	}
}