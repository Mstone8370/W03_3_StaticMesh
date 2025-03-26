#include "pch.h" 
#include "PlayerController.h"

#include "Static/EditorManager.h"
#include "PlayerInput.h"
#include "Core/Math/Plane.h"
#include "Editor/Slate/SSplitter.h"
#include "Editor/Viewport/FViewport.h"
#include "Engine/Engine.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Engine/EngineConfig.h"
#include "Rendering/URenderer.h"

APlayerController::APlayerController()
    : CurrentSpeed(5.f)
    , MaxSpeed(30.f)
    , MinSpeed(1.0f)
    , MouseSensitivity(0.2f)
    , MaxSensitivity(0.5f)
    , MinSensitivity(0.05f)
{}

APlayerController::~APlayerController()
{
}

void APlayerController::HandleCameraMovement(ACamera* Camera, bool bIsPerspective, float DeltaTime)
{
	FVector MoveDir = GetCameraMovementDirection(Camera, bIsPerspective);
	if (MoveDir.Length() > SMALL_NUMBER)
	{
		MoveDir = MoveDir.GetSafeNormal();
		FTransform Transform = Camera->GetActorTransform();
		Transform.Translate(MoveDir * DeltaTime * CurrentSpeed);
		Camera->SetActorTransform(Transform);
	}

	int32 WheelValue = APlayerInput::Get().GetMouseWheelDelta();
	CurrentSpeed += static_cast<int>(WheelValue * 0.01f);
	CurrentSpeed = FMath::Clamp(CurrentSpeed, MinSpeed, MaxSpeed);
}

void APlayerController::HandleCameraRotation(ACamera* Camera, bool bIsPerspective)
{
	if (!APlayerInput::Get().IsMouseDown(true))
	{
		return;
	}

	int32 DeltaX = 0;
	int32 DeltaY = 0;
	APlayerInput::Get().GetMouseDelta(DeltaX, DeltaY);

	FTransform CameraTransform = Camera->GetActorTransform();

	if (bIsPerspective)
	{
		FVector Euler = CameraTransform.GetRotation().GetEuler();
		Euler.Z += DeltaX * MouseSensitivity; // Yaw
		Euler.Y += DeltaY * MouseSensitivity; // Pitch
		Euler.Y = FMath::Clamp(Euler.Y, -Camera->GetMaxPitch(), Camera->GetMaxPitch());
		CameraTransform.SetRotation(Euler);
	}
	else
	{
		// Ortho: 마우스 드래그 → 패닝
		FVector Right = Camera->GetActorRightVector();
		FVector Up = Camera->GetActorUpVector();
		FVector Offset = (-Right * DeltaX + Up * DeltaY) * 0.01f;
		CameraTransform.Translate(Offset);
	}

	Camera->SetActorTransform(CameraTransform);
	APlayerInput::Get().FixMouseCursor();
}

void APlayerController::HandleZoom()
{
	float WheelDelta = APlayerInput::Get().GetMouseWheelDelta();
	if (FMath::Abs(WheelDelta) < KINDA_SMALL_NUMBER)
		return;

	int32 ViewportIndex = GetHoveredViewportIndex();
	if (ViewportIndex == -1)
		return;

	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (!Renderer || ViewportIndex >= Renderer->Viewports.Num())
		return;

	SViewport* SView = Renderer->Viewports[ViewportIndex];
	FViewport* FView = SView->GetFViewport();
	if (!FView || !FView->GetCamera())
		return;

	ACamera* Camera = FView->GetCamera();
	FTransform CameraTransform = Camera->GetActorTransform();

	const float ZoomSpeed = 0.5f;
	WheelDelta = FMath::Clamp(WheelDelta, -1.0f, 1.0f);
	if (Camera->GetProjectionMode() == ECameraProjectionMode::Perspective)
	{
		// Perspective: 카메라 이동
		FVector Forward = CameraTransform.GetForward();
		CameraTransform.Translate(Forward * WheelDelta * ZoomSpeed);
		Camera->SetActorTransform(CameraTransform);
	}
	else
	{
		// Ortho: 폭 조절
		float Width = Camera->GetOrthoWidth();
		float NewWidth = FMath::Clamp(Width * (1.f - WheelDelta * ZoomSpeed), 1.0f, 500.0f);
		//float Aspect = SView->GetRect().Width/SView->GetRect().Height;
		//float Height = NewWidth / Aspect;

		Camera->SetOrthoSize(NewWidth, SView->GetRect());
	}

	// 카메라 변경 사항 반영
	//FEditorManager::Get().SetMainCamera(Camera);
	UEngine::Get().GetRenderer()->UpdateProjectionMatrix(Camera);
}

FVector APlayerController::GetCameraMovementDirection(ACamera* Camera, bool bIsPerspective)
{
	FVector Dir = FVector::Zero();

	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::W))
	{
		Dir += bIsPerspective ? Camera->GetActorForwardVector() : Camera->GetActorUpVector();
	}
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::S))
	{
		Dir -= bIsPerspective ? Camera->GetActorForwardVector() : Camera->GetActorUpVector();
	}
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::D))
	{
		Dir += Camera->GetActorRightVector();
	}
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::A))
	{
		Dir -= Camera->GetActorRightVector();
	}
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::Q))
	{
		Dir -= FVector(0, 0, 1);
	}
	if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::E))
	{
		Dir += FVector(0, 0, 1);
	}

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

    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosX, CameraTransform.GetLocation().X);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosY, CameraTransform.GetLocation().Y);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosZ, CameraTransform.GetLocation().Z);

    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotX, CameraTransform.GetRotation().X);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotY, CameraTransform.GetRotation().Y);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotZ, CameraTransform.GetRotation().Z);
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotW, CameraTransform.GetRotation().W);
	
    UEngine::Get().GetEngineConfig()->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraSpeed, CameraSpeed);
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
	if (HandleUiCapture())
	{
		return;
	}

	UpdateViewportClickState();
	HandleCursorLock();
	HandleZoom();

	int32 ViewportIndex = GetActiveViewportIndex();
	if (ViewportIndex == -1)
	{
		return;
	}

	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (!Renderer || ViewportIndex >= Renderer->Viewports.Num())
	{
		return;
	}

	SViewport* SView = Renderer->Viewports[ViewportIndex];
	FViewport* FView = SView->GetFViewport();
	if (!FView)
	{
		return;
	}

	ACamera* Camera = FView->GetCamera();
	if (!Camera)
	{
		return;
	}
	
	bool bIsPerspective = Camera->GetProjectionMode() == ECameraProjectionMode::Perspective;

	HandleCameraRotation(Camera, bIsPerspective);
	HandleCameraMovement(Camera, bIsPerspective, DeltaTime);

	if (bIsPerspective)
	{
		SaveCameraProperties(Camera);
	}
}

bool APlayerController::HandleUiCapture()
{
	if (bUiInput)
	{
		if (APlayerInput::Get().IsMouseDown(true) || APlayerInput::Get().IsMouseDown(false))
			bUiCaptured = true;
		return true;
	}

	if (bUiCaptured)
	{
		if (!APlayerInput::Get().IsMouseDown(true) && !APlayerInput::Get().IsMouseDown(false))
			bUiCaptured = false;
		return true;
	}

	return false;
}

void APlayerController::HandleCursorLock()
{
	if (APlayerInput::Get().IsMousePressed(true))
	{
		APlayerInput::Get().CacheCursorPosition();
		ShowCursor(false);
	}

	if (APlayerInput::Get().IsMouseReleased(true))
	{
		ShowCursor(true);
	}
}

bool APlayerController::HandleViewportDrag(float ViewportWidth, float ViewportHeight)
{
	bool bResult = false;
	int32 MouseX, MouseY;
	APlayerInput::Get().GetMousePositionClient(MouseX, MouseY);

	URenderer* Renderer = UEngine::Get().GetRenderer();
	SSplitter* Root = dynamic_cast<SSplitter*>(Renderer->RootWindow);
	if (!Root)
	{
		return false;
	}

	// Get Top and Bottom horizontal splitters
	SSplitter* Top = dynamic_cast<SSplitter*>(Root->GetChild(0));
	SSplitter* Bottom = dynamic_cast<SSplitter*>(Root->GetChild(1));
	if (!Top || !Bottom)
	{
		return false;
	}

	// 스플리터 기준 분할 위치
	float DragX_Top = ViewportWidth * Top->GetRatio();
	float DragX_Bottom = ViewportWidth * Bottom->GetRatio();
	float DragY = ViewportHeight * Root->GetRatio();

	bool bHoverTop = abs(MouseX - DragX_Top) <= DragHandleSize && MouseY < DragY+DragHandleSize;
	bool bHoverBottom = abs(MouseX - DragX_Bottom) <= DragHandleSize && MouseY >= DragY-DragHandleSize;

	bool bHoverVertical = abs(MouseY - DragY) <= DragHandleSize;

	if (APlayerInput::Get().IsMousePressed(false))
	{
		if (bHoverTop)
		{
			bDraggingTop = true;
		}
		if (bHoverBottom)
		{
			bDraggingBottom = true;
		}
		if (bHoverVertical)
		{
			bDraggingVertical = true;
		}
	}
	else if (!APlayerInput::Get().IsMouseDown(false))
	{
		bDraggingTop = false;
		bDraggingBottom = false;
		bDraggingVertical = false;
	}

	if (bDraggingTop)
	{
		float Ratio = static_cast<float>(MouseX) / ViewportWidth;
		Top->SetRatio(Ratio);
		bResult = true;
		if (abs(Bottom->GetRatio() - Top->GetRatio())<=0.003f)Top->SetRatio(Bottom->GetRatio());
	}

	if (bDraggingBottom)
	{
		float Ratio = static_cast<float>(MouseX) / ViewportWidth;
		Bottom->SetRatio(Ratio);
		bResult = true;
		if (abs(Bottom->GetRatio() - Top->GetRatio())<=0.003f)Bottom->SetRatio(Top->GetRatio());
	}

	if (bDraggingVertical)
	{
		float Ratio = static_cast<float>(MouseY) / ViewportHeight;
		Root->SetRatio(Ratio);
		bResult = true;
	}
	if (bResult)
	{
		FEditorManager::Get().SaveSplitterLayout(Root);
	}
	return bResult;
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
	
int32 APlayerController::GetHoveredViewportIndex() const
{
	int32 MouseX, MouseY;
	APlayerInput::Get().GetMousePositionClient(MouseX, MouseY);

	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (!Renderer)
		return -1;

	const TArray<SViewport*>& Viewports = Renderer->Viewports;
	for (int32 i = 0; i < Viewports.Num(); ++i)
	{
		const SViewport* SView = Viewports[i];
		if (!SView) continue;

		const FRect& Rect = SView->GetRect();
		if (Rect.Contains(FPoint(MouseX, MouseY)))
		{
			return i;
		}
	}

	return -1; // 호버된 뷰포트 없음
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
