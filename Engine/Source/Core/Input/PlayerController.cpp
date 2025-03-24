#include "pch.h" 
#include "PlayerController.h"

#include "Static/EditorManager.h"
#include "PlayerInput.h"
#include "Core/Math/Plane.h"
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
	if (bUiCaptured || !APlayerInput::Get().IsMouseDown(true))
	{
		if (APlayerInput::Get().IsMouseReleased(true))
		{
			ShowCursor(true);
		}
		return;
	}

	int32 ViewportIndex = GetClickedViewportIndex();
	if (ViewportIndex == -1)
		return;

	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (ViewportIndex >= Renderer->Viewports.Num())
		return;

	ACamera* Camera = Renderer->Viewports[ViewportIndex].ViewCamera;
	if (!Camera)
		return;

	FTransform CameraTransform = Camera->GetActorTransform();
	const bool bIsPerspective = Camera->GetProjectionMode() == ECameraProjectionMode::Perspective;

	// 회전 처리
	int32 DeltaX = 0, DeltaY = 0;
	APlayerInput::Get().GetMouseDelta(DeltaX, DeltaY);
	if (bIsPerspective)
	{
		// 회전 처리 (Yaw, Pitch)
		FVector Euler = CameraTransform.GetRotation().GetEuler();
		Euler.Y += MouseSensitivity * static_cast<float>(DeltaY); // Pitch
		Euler.Z += MouseSensitivity * static_cast<float>(DeltaX); // Yaw
		Euler.Y = FMath::Clamp(Euler.Y, -Camera->GetMaxPitch(), Camera->GetMaxPitch());
		CameraTransform.SetRotation(Euler);
	}
	else
	{
		// Ortho 모드에서는 카메라를 XY 평면 기준으로 이동 (패닝)
		FVector Right = Camera->GetActorRightVector();
		FVector Up = Camera->GetActorUpVector();

		FVector Offset = (-Right * static_cast<float>(DeltaX) + Up * static_cast<float>(DeltaY)) * 0.01f;
		CameraTransform.Translate(Offset);
	}

	// 커서 고정
	if (APlayerInput::Get().IsMousePressed(true))
	{
		APlayerInput::Get().CacheCursorPosition();
		ShowCursor(false);
	}
	APlayerInput::Get().FixMouseCursor();

	// 이동 처리 (WASDQE)
	FVector NewVelocity = FVector::Zero();
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::A))
		NewVelocity -= Camera->GetActorRightVector();
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::D))
		NewVelocity += Camera->GetActorRightVector();
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::W))
		NewVelocity += bIsPerspective ? Camera->GetActorForwardVector() : Camera->GetActorUpVector();
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::S))
		NewVelocity -= bIsPerspective ? Camera->GetActorForwardVector() : Camera->GetActorUpVector();
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::Q))
		NewVelocity -= FVector(0, 0, 1);
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::E))
		NewVelocity += FVector(0, 0, 1);

	if (NewVelocity.Length() > 0.001f)
		NewVelocity = NewVelocity.GetSafeNormal();

	CameraTransform.Translate(NewVelocity * DeltaTime * CurrentSpeed);
	Camera->SetActorTransform(CameraTransform);

	SaveCameraProperties(Camera);
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
    
    // TODO: 기즈모 조작시에는 카메라 입력 무시
    // HandleGizmoMovement(DeltaTime); // TODO: 의미없는 함수인듯
    HandleCameraMovement(DeltaTime);
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

    const TArray<FViewport>& Viewports = Renderer->Viewports;

    for (int32 i = 0; i < Viewports.Num(); ++i)
    {
        const FViewport& View = Viewports[i];
		
        float Left = View.Position.X;
        float Top = View.Position.Y;
        float Right = Left + View.Size.X;
        float Bottom = Top + View.Size.Y;

        // 마우스가 이 뷰포트 영역 안에 있는가?
        if (MouseX >= Left && MouseX < Right &&
            MouseY >= Top && MouseY < Bottom)
        {
            return i;
        }
    }

    return -1; // 어떤 뷰포트도 클릭되지 않음
}
