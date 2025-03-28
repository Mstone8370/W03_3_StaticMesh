#pragma once

#include "Core/AbstractClass/Singleton.h"
#include "Math/Vector.h"

class ACamera;
struct FTransform;

class APlayerController : public TSingleton<APlayerController>
{
	// TODO: 카메라는 플레이어 컨트롤러가 가지고있어야 함.
	
public :
	APlayerController();
	~APlayerController();

	void ProcessPlayerInput(float DeltaTime);
	bool HandleUiCapture();
	void HandleCursorLock();

	void HandleCameraMovement(ACamera* Camera, bool bIsPerspective, float DeltaTime);
	void SaveCameraProperties(class ACamera* Camera);
	void HandleCameraRotation(ACamera* Camera, bool bIsPerspective);
	void HandleZoom();
	FVector GetCameraMovementDirection(ACamera* Camera, bool bIsPerspective);

	float GetCurrentSpeed() const { return CurrentSpeed; }
	float GetMaxSpeed() const { return MaxSpeed; }
	float GetMinSpeed() const { return MinSpeed; }

	void SetCurrentSpeed(float InSpeed);

	float GetMouseSensitivity() const { return MouseSensitivity; }
	float GetMaxSensitivity() const { return MaxSensitivity; }
	float GetMinSensitivity() const { return MinSensitivity; }
	
	void SetMouseSensitivity(float InSensitivity);

	// TODO: 함수랑 변수 이름 맘에 안듬
	bool IsUiInput() const { return bUiInput; }
	void SetIsUiInput(bool bInUiInput) { bUiInput = bInUiInput; }

	//ViewPort 관련
    int32 ClickedViewportIndex = -1;
	float DragHandleSize = 10.f;
	bool bDraggingHorizontal = false;
	//bool bDraggingVertical = false;
	bool HandleViewportDrag(float ViewportWidth, float ViewportHeight);
	int32 GetClickedViewportIndex();
	int32 GetActiveViewportIndex() const { return ClickedViewportIndex; }
	int32 GetHoveredViewportIndex() const;

	void UpdateViewportClickState();
protected:
	float CurrentSpeed;
	float MaxSpeed;
	float MinSpeed;

	float MouseSensitivity;
	float MaxSensitivity;
	float MinSensitivity;

	bool bIsHandlingGizmo;

	bool bUiInput = false;
	bool bUiCaptured = false;

	bool bDraggingTop = false;
	bool bDraggingBottom = false;
	bool bDraggingVertical = false;
};
