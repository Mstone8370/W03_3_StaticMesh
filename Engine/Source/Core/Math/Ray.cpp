#include "pch.h"
#include "Ray.h"
#include "GameFramework/Camera.h"
#include "Editor/Viewport/Viewport.h"
FRay FRay::GetRayByMousePoint(ACamera* InCamera)
{
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
FRay FRay::GetRayByViewportPoint(ACamera* InCamera, const FViewport& Viewport)
{
	if (InCamera == nullptr)
	{
		return FRay();
	}

	// 1. 마우스 커서 위치 얻기 (Screen to Client → Viewport 상대 좌표)
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

	float LocalX = static_cast<float>(pt.x) - Viewport.Position.X;
	float LocalY = static_cast<float>(pt.y) - Viewport.Position.Y;

	float NDCX = 2.0f * LocalX / Viewport.Size.X - 1.0f;
	float NDCY = -2.0f * LocalY / Viewport.Size.Y + 1.0f;

	FVector4 RayOrigin(NDCX, NDCY, 0.0f, 1.0f);
	FVector4 RayEnd(NDCX, NDCY, 1.0f, 1.0f);

	// 2. Projection 공간 → View 공간
	FMatrix InvProjMat = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
	RayOrigin = InvProjMat.TransformVector4(RayOrigin);
	RayOrigin.W = 1.0f;
	RayEnd = InvProjMat.TransformVector4(RayEnd);
	RayEnd *= 1000.f;
	RayEnd.W = 1.0f;

	// 3. View 공간 → 월드 공간
	FMatrix InvViewMat = InCamera->GetViewMatrix().Inverse();
	RayOrigin = InvViewMat.TransformVector4(RayOrigin);
	RayEnd = InvViewMat.TransformVector4(RayEnd);

	FVector Delta = RayEnd - RayOrigin;
	FVector Dir = Delta.GetSafeNormal();
	float Length = Delta.Length();

	return FRay(RayOrigin, Dir, Length);
}
