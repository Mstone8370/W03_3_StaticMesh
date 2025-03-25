#include "pch.h"
#include "Ray.h"

#include "Editor/Viewport/FViewport.h"
#include "GameFramework/Camera.h"
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
FRay FRay::GetRayByViewportPoint(ACamera* InCamera, const FRect& ViewRect)
{
	if (!InCamera || ViewRect.Width <= 0.f || ViewRect.Height <= 0.f)
		return FRay();

	// 1. 마우스 → Viewport 로컬 좌표
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

	float LocalX = static_cast<float>(pt.x) - ViewRect.X;
	float LocalY = static_cast<float>(pt.y) - ViewRect.Y;

	// 2. 로컬 → NDC
	float NDCX = 2.0f * LocalX / ViewRect.Width - 1.0f;
	float NDCY = -2.0f * LocalY / ViewRect.Height + 1.0f;

	FVector4 RayOriginNDC(NDCX, NDCY, 0.0f, 1.0f);
	FVector4 RayEndNDC(NDCX, NDCY, 1.0f, 1.0f);

	// 3. Projection → View
	FMatrix InvProj = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
	FVector4 RayOriginView = InvProj.TransformVector4(RayOriginNDC);
	FVector4 RayEndView = InvProj.TransformVector4(RayEndNDC);
	RayEndView *= 1000.f;
	
	// 4. View → World
	FMatrix InvView = InCamera->GetViewMatrix().Inverse();
	FVector4 RayOriginWorld = InvView.TransformVector4(RayOriginView);
	FVector4 RayEndWorld = InvView.TransformVector4(RayEndView);

	FVector Delta = RayEndWorld - RayOriginWorld;
	return FRay(RayOriginWorld, Delta.GetSafeNormal(), Delta.Length());
}
