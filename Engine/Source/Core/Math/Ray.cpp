#include "pch.h"
#include "Ray.h"

#include "Editor/Viewport/FViewport.h"
#include "GameFramework/Camera.h"
#include "Input/PlayerInput.h"

FRay FRay::GetRayByMousePoint(ACamera* InCamera)
{
	//뷰포트 추가 후 사용 안함
    if(InCamera == nullptr)
	{
		return FRay();
	}
    // 1. 마우스 커서 위치를 NDC로 변환
    POINT pt;
    GetCursorPos(&pt);

    ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);
    float ScreenWidth = UEngine::Get().GetClientWidth();
    float ScreenHeight = UEngine::Get().GetClientHeight();

    float NDCX = 2.0f * pt.x / ScreenWidth - 1.0f;
    float NDCY = -2.0f * pt.y / ScreenHeight + 1.0f;

    // 2. Projection 공간으로 변환
    FVector4 RayOrigin = FVector4(NDCX, NDCY, 0.0f, 1.0f);
    FVector4 RayEnd = FVector4(NDCX, NDCY, 1.0f, 1.0f);

    // 3. View 공간으로 변환
    FMatrix InvProjMat = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
    RayOrigin = InvProjMat.TransformVector4(RayOrigin);
    RayOrigin.W = 1;
    RayEnd = InvProjMat.TransformVector4(RayEnd);
    RayEnd *= 1000.f; // TODO: 임의 값인 경우 카메라의 FarClip에 맞게 변경하기.
    RayEnd.W = 1;

    // 4. 월드 공간으로 변환
    FMatrix InvViewMat = InCamera->GetViewMatrix().Inverse();
    RayOrigin = InvViewMat.TransformVector4(RayOrigin);
    //RayOrigin /= RayOrigin.W = 1;
    RayEnd = InvViewMat.TransformVector4(RayEnd);
	FVector RayDelta = RayEnd - RayOrigin;
    FVector RayDir = RayDelta.GetSafeNormal();
	float RayLength = RayDelta.Length();
 
	return FRay(RayOrigin, RayDir, RayLength);
}
FRay FRay::GetRayByViewportPoint(ACamera* InCamera, const FRect& ViewRect)
{
	if (!InCamera || ViewRect.Width <= 0.f || ViewRect.Height <= 0.f)
		return FRay();

	// 1. 마우스 → Viewport 로컬 좌표
	int X, Y;
	APlayerInput::Get().GetMousePositionClient(X, Y);
	FPoint pt(X, Y);

	float LocalX = pt.X - ViewRect.X;
	float LocalY = pt.Y - ViewRect.Y;

	// 2. 유효 영역 체크
	if (LocalX < 0 || LocalY < 0 || LocalX > ViewRect.Width || LocalY > ViewRect.Height)
		return FRay();

	// 3. 로컬 → NDC (-1 ~ +1)
	float NDCX = 2.0f * LocalX / ViewRect.Width - 1.0f;
	float NDCY = -2.0f * LocalY / ViewRect.Height + 1.0f;

	FVector4 RayClipNear(NDCX, NDCY, 0.0f, 1.0f);
	FVector4 RayClipFar (NDCX, NDCY, 1.0f, 1.0f);

	// 4. Projection / View Matrix 역변환
	float AspectRatio = ViewRect.Width / ViewRect.Height;
	FMatrix InvProj = InCamera->GetProjectionMatrix(AspectRatio).Inverse();
	FMatrix InvView = InCamera->GetViewMatrix().Inverse();

	// 5. NDC → View → World
	FVector4 WorldNear = InvView.TransformVector4(InvProj.TransformVector4(RayClipNear));
	FVector4 WorldFar  = InvView.TransformVector4(InvProj.TransformVector4(RayClipFar));
	WorldFar *= 1000.f;

	// 6. 카메라 모드에 따른 Ray 구성
	if (InCamera->GetProjectionMode() == ECameraProjectionMode::Orthographic)
	{
		FVector Origin = FVector(WorldNear.X, WorldNear.Y, WorldNear.Z);
		FVector Dir = InCamera->GetActorTransform().GetForward();
		return FRay(Origin, Dir, 10000.f);
	}
	else // Perspective
	{
		FVector RayStart = InCamera->GetActorTransform().GetPosition();
		FVector RayEnd = FVector(WorldFar.X, WorldFar.Y, WorldFar.Z);
		FVector Dir = (RayEnd - RayStart).GetSafeNormal();
		return FRay(RayStart, Dir, (RayEnd - RayStart).Length());
	}
}
