#include "pch.h" 
#include "GizmoHandle.h"

#include "Engine/GameFrameWork/Camera.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "CoreUObject/World.h"
#include "Core/Input/PlayerInput.h"
#include "Static/EditorManager.h"
#include "Static/ObjManager.h"
#include "Components/StaticMeshComponent.h"
#include "Input/PlayerController.h"

AGizmoHandle::AGizmoHandle()
{
	bIsGizmo = true;
	bUseBoundingBox = true;
	bRenderBoundingBox = false;

    USceneComponent* Root = AddComponent<USceneComponent>();
    RootComponent = Root;

    InitTranslationGizmo();
    InitRotationGizmo();
    InitScaleGizmo();

    OnGizmoTypeChanged(GizmoType);
    
    SetActive(false);
}

void AGizmoHandle::InitTranslationGizmo()
{
    // x
    UStaticMesh* SM_Translation = FObjManager::LoadObjStaticMesh(TEXT("Resources/GizmoTranslation.obj"));
    UStaticMeshComponent* TranslationX = AddComponent<UStaticMeshComponent>();
    TranslationX->SetMeshName("GizmoTranslation");
    TranslationX->SetStaticMesh(SM_Translation);
    TranslationX->SetCanBeRendered(true);
    TranslationX->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), GizmoScale));
    TranslationX->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    AllGizmos.Add(TranslationX);
    TranslationGizmos.Add(TranslationX);
    TranslationX->SetupAttachment(RootComponent);

    // y
    UStaticMeshComponent* TranslationY = AddComponent<UStaticMeshComponent>();
    TranslationY->SetMeshName("GizmoTranslation");
    TranslationY->SetStaticMesh(SM_Translation);
    TranslationY->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 90.0f), GizmoScale));
    TranslationY->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    AllGizmos.Add(TranslationY);
    TranslationGizmos.Add(TranslationY);
    TranslationY->SetupAttachment(RootComponent);

    // z
    UStaticMeshComponent* TranslationZ = AddComponent<UStaticMeshComponent>();
    TranslationZ->SetMeshName("GizmoTranslation");
    TranslationZ->SetStaticMesh(SM_Translation);
    TranslationZ->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, 0.0f), GizmoScale));
    TranslationZ->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
    AllGizmos.Add(TranslationZ);
    TranslationGizmos.Add(TranslationZ);
    TranslationZ->SetupAttachment(RootComponent);

    FString ObjPath = SM_Translation->GetAssetPathFileName();
    UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);

}

void AGizmoHandle::InitRotationGizmo()
{
    // x
    UStaticMeshComponent* RotationX = AddComponent<UStaticMeshComponent>();
    UStaticMesh* SM_Rotation = FObjManager::LoadObjStaticMesh(TEXT("Resources/GizmoRotation.obj"));
    RotationX->SetStaticMesh(SM_Rotation);
    RotationX->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), GizmoScale));
    RotationX->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    AllGizmos.Add(RotationX);
    RotationGizmos.Add(RotationX);
    RotationX->SetupAttachment(RootComponent);

    // y
    UStaticMeshComponent* RotationY = AddComponent<UStaticMeshComponent>();
    RotationY->SetStaticMesh(SM_Rotation);
    RotationY->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90.0f, 0.0f, 90.0f), GizmoScale));
    RotationY->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    AllGizmos.Add(RotationY);
    RotationGizmos.Add(RotationY);
    RotationY->SetupAttachment(RootComponent);

    // z
    UStaticMeshComponent* RotationZ = AddComponent<UStaticMeshComponent>();
    RotationZ->SetStaticMesh(SM_Rotation);
    RotationZ->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, -90.0f), GizmoScale));
    RotationZ->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
    AllGizmos.Add(RotationZ);
    RotationGizmos.Add(RotationZ);
    RotationZ->SetupAttachment(RootComponent);
   
    FString ObjPath = SM_Rotation->GetAssetPathFileName();
    UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);
}

void AGizmoHandle::InitScaleGizmo()
{
    // x
    UStaticMesh* SM_Scale = FObjManager::LoadObjStaticMesh(TEXT("Resources/GizmoScale.obj"));
    UStaticMeshComponent* ScaleX = AddComponent<UStaticMeshComponent>();
    ScaleX->SetStaticMesh(SM_Scale);
    ScaleX->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), GizmoScale));
    ScaleX->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
    AllGizmos.Add(ScaleX);
    ScaleGizmos.Add(ScaleX);
    ScaleX->SetupAttachment(RootComponent);

    // y
    UStaticMeshComponent* ScaleY = AddComponent<UStaticMeshComponent>();
    ScaleY->SetStaticMesh(SM_Scale);
    ScaleY->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 90.0f), GizmoScale));
    ScaleY->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
    AllGizmos.Add(ScaleY);
    ScaleGizmos.Add(ScaleY);
    ScaleY->SetupAttachment(RootComponent);

    // z
    UStaticMeshComponent* ScaleZ = AddComponent<UStaticMeshComponent>();
    ScaleZ->SetStaticMesh(SM_Scale);
    ScaleZ->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, 0.0f), GizmoScale));
    ScaleZ->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
    AllGizmos.Add(ScaleZ);
    ScaleGizmos.Add(ScaleZ);
    ScaleZ->SetupAttachment(RootComponent);
  
    FString ObjPath = SM_Scale->GetAssetPathFileName();
    UEngine::Get().GetRenderer()->GetBufferCache()->BuildStaticMesh(ObjPath);
}

void AGizmoHandle::OnGizmoTypeChanged(EGizmoType NewGizmoType)
{
    HideAllGizmo();

    switch (NewGizmoType)
    {
    case EGizmoType::Translate:
        for (auto& Gizmo : TranslationGizmos)
        {
            Gizmo->SetCanBeRendered(true);
        }
        break;
    case EGizmoType::Rotate:
        for (auto& Gizmo : RotationGizmos)
        {
            Gizmo->SetCanBeRendered(true);
        }
        break;
    case EGizmoType::Scale:
        for (auto& Gizmo : ScaleGizmos)
        {
            Gizmo->SetCanBeRendered(true);
        }
        break;
    default:
        break;
    }
}

void AGizmoHandle::HideAllGizmo()
{
    for (auto& Gizmo : AllGizmos)
    {
        Gizmo->SetCanBeRendered(false);
    }
}

void AGizmoHandle::Tick(float DeltaTime)
{
	if (!bIsActive)
	{
		return;
	}
	
	USceneComponent* SelectedComponent = FEditorManager::Get().GetSelectedComponent();
    if (SelectedComponent != nullptr && bIsActive)
    {
        FTransform GizmoTr = RootComponent->GetComponentTransform();
        GizmoTr.SetPosition(SelectedComponent->GetWorldTransform().GetLocation());
		if (bIsLocal)
		{
            GizmoTr.SetRotation(SelectedComponent->GetWorldTransform().GetRotation());
		}
        else
        {
			GizmoTr.SetRotation(FQuat());
        }
        SetActorTransform(GizmoTr);
    }

    SetScaleByDistance();

    AActor::Tick(DeltaTime);

	if (APlayerInput::Get().IsMouseReleased(false))
	{
		SelectedAxis = ESelectedAxis::None;
		CachedRayResult = FVector::ZeroVector;
	}
    if (SelectedAxis != ESelectedAxis::None)
    {
        if (USceneComponent* SceneComp = FEditorManager::Get().GetSelectedComponent())
        {
            // 마우스의 커서 위치를 가져오기
        	float PosX;
        	float PosY;
        	APlayerInput::Get().GetMousePositionNDC(PosX, PosY);
        	
			FVector4 RayOrigin{ PosX, PosY, 0.0f, 1.0f };
			FVector4 RayEnd{ PosX, PosY, 1.0f, 1.0f };

			FMatrix InvProjMat = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
			RayOrigin = InvProjMat.TransformVector4(RayOrigin);
			RayOrigin.W = 1;
			RayEnd = InvProjMat.TransformVector4(RayEnd);
			RayEnd *= 1000.0f;  
			RayEnd.W = 1;

			FMatrix InvViewMat = FEditorManager::Get().GetMainCamera()->GetViewMatrix().Inverse();
			RayOrigin = InvViewMat.TransformVector4(RayOrigin);
			RayOrigin /= RayOrigin.W = 1;
			RayEnd = InvViewMat.TransformVector4(RayEnd);
			RayEnd /= RayEnd.W = 1;
			FVector RayDir = (RayEnd - RayOrigin).GetSafeNormal();

			float Distance = FVector::Distance(RayOrigin, SceneComp->GetComponentTransform().GetLocation());

            // 이전 프레임의 Result가 있어야 함
			FVector Result = RayOrigin + RayDir * Distance;

			if (CachedRayResult == FVector::ZeroVector)
			{
				CachedRayResult = Result;
                return;
			}

			FVector Delta = Result - CachedRayResult;
            CachedRayResult = Result;
            FTransform CompTransform = SceneComp->GetComponentTransform();

			DoTransform(CompTransform, Delta, SceneComp);
		}
        else
        {
			CachedRayResult = FVector::ZeroVector;
        }
	}

    if (!APlayerController::Get().IsUiInput() && APlayerInput::Get().IsKeyPressed(DirectX::Keyboard::Keys::Space))
    {
        int type = static_cast<int>(GizmoType);
        type = (type + 1) % static_cast<int>(EGizmoType::Max);
        GizmoType = static_cast<EGizmoType>(type);
        OnGizmoTypeChanged(GizmoType);
    }

}

void AGizmoHandle::SetScaleByDistance()
{
    FTransform MyTransform = GetActorTransform();

    // 액터의 월드 위치
    FVector ActorWorldLocation = MyTransform.GetLocation();
	
    FTransform CameraTransform = FEditorManager::Get().GetMainCamera()->GetActorTransform();

    // 카메라의 월드 위치
    FVector CameraWorldLocation = CameraTransform.GetLocation();

    // 거리 계산
    float Distance = (ActorWorldLocation - CameraWorldLocation).Length();

    float BaseScale = 3.0f;    // 기본 스케일
    float ScaleFactor = Distance / BaseScale; // 거리에 비례하여 스케일 증d가

    // float minScale = 1.0f;     // 최소 스케일
    // float maxScale = 1.0f;     // 최대 스케일
    // float scaleFactor = clamp(1.0f / distance, minScale, maxScale);

    MyTransform.SetScale(ScaleFactor, ScaleFactor, ScaleFactor);
    SetActorTransform(MyTransform);
}

void AGizmoHandle::SetActive(bool bActive)
{
    bIsActive = bActive;
    if (bIsActive)
    {
        OnGizmoTypeChanged(GizmoType);
    }
    else
    {
        HideAllGizmo();
    }
}

const char* AGizmoHandle::GetTypeName()
{
    return "GizmoHandle";
}

void AGizmoHandle::DoTransform(FTransform& CompTransform, FVector Delta, USceneComponent* SceneComp)
{
    FVector WorldDirection;
    FVector LocalDirection;

	FVector CamToComp = (SceneComp->GetComponentTransform().GetLocation() - FEditorManager::Get().GetMainCamera()->GetActorTransform().GetLocation()).GetSafeNormal();
	FVector RotationDelta = FVector::CrossProduct(Delta, CamToComp);

	float DeltaLength = Delta.Length();
    switch (SelectedAxis)
    {
	case ESelectedAxis::X:
		WorldDirection = FVector(1, 0, 0);
        LocalDirection = CompTransform.GetForward();
		break;
	case ESelectedAxis::Y:
		WorldDirection = FVector(0, 1, 0);
        LocalDirection = CompTransform.GetRight();
		break;
	case ESelectedAxis::Z:
		WorldDirection = FVector(0, 0, 1);
		LocalDirection = CompTransform.GetUp();
        break;
    }

	bool bIsLocal = FEditorManager::Get().GetGizmoHandle()->bIsLocal;
	FVector Direction = bIsLocal ? LocalDirection : WorldDirection;

	Delta = Direction * FVector::DotProduct(Delta, Direction);

	int Sign = FVector::DotProduct(RotationDelta, Direction) > 0 ? 1 : -1;

	switch (GizmoType)
	{
	case EGizmoType::Translate:
		CompTransform.Translate(Delta);
		break;
	case EGizmoType::Rotate:
		CompTransform.Rotate(Direction, Sign * DeltaLength * 50);
		break;
	case EGizmoType::Scale:
        // 스케일은 축에 평행한 방향으로 커지는게 맞음
		Delta = WorldDirection * FVector::DotProduct(Delta, WorldDirection) * 2;
		CompTransform.AddScale(Delta);
		break;
	}
	SceneComp->SetRelativeTransform(CompTransform);
}

