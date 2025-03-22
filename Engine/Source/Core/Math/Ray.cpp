#include "pch.h"
#include "Ray.h"
#include "GameFramework/Camera.h"
#include "Input/PlayerInput.h"

FRay FRay::GetRayByMousePoint(const ACamera* InCamera)
{
    if(InCamera == nullptr)
	{
		return FRay();
	}
	
    float NdcX = 0.f;
    float NcdY = 0.f;
	APlayerInput::Get().GetMousePositionNDC(NdcX, NcdY); // TODO: 다중 뷰포트인 경우 NDC가 아닌 뷰포트 기준으로

    // 2. Projection 공간으로 변환
    FVector4 RayOrigin = FVector4(NdcX, NcdY, 0.0f, 1.0f);
    FVector4 RayEnd = FVector4(NdcX, NcdY, 1.0f, 1.0f);

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
