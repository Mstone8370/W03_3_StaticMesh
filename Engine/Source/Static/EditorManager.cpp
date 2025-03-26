#include "pch.h"
#include "EditorManager.h"
#include "Engine/Engine.h"
#include "CoreUObject/World.h"
#include "Gizmo/GizmoHandle.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Transform.h"
#include "Editor/Slate/SSplitter.h"
#include "Engine/GameFrameWork/Camera.h"

void FEditorManager::SelectActor(AActor* NewActor)
{
    if (NewActor == nullptr)
    {
        return;
    };

    if (GizmoHandle == nullptr)
    {
        GizmoHandle = UEngine::Get().GetWorld()->SpawnActor<AGizmoHandle>();
        GizmoHandle->SetActive(false);
    }

    if (SelectedActor != nullptr)
        SelectedActor->UnPick();

    SelectedActor = NewActor;
    SelectedActor->Pick();
    GizmoHandle->SetActive(true);
}

void FEditorManager::SetMainCamera(ACamera* NewCamera)
{
    MainCamera = NewCamera;
    ViewportCameras[EEditorViewportType::Perspective] = NewCamera;
}

void FEditorManager::ToggleGizmoHandleLocal(bool bIsLocal)
{
    if (GizmoHandle)
        GizmoHandle->SetLocal(bIsLocal);
}

USceneComponent* FEditorManager::GetSelectedComponent() const
{
    if (SelectedComponent == nullptr)
    {
        if (SelectedActor)
            return SelectedActor->GetRootComponent();

        return nullptr;
    }
    return SelectedComponent;
}

void FEditorManager::SelectComponent(USceneComponent* InSelectedComponent)
{
    if (InSelectedComponent == nullptr)
    {
        return;
    }

    // 선택된 액터가 없는 시점에 호출되면 SelectActor호출
    if (SelectedActor == nullptr)
    {
        SelectActor(InSelectedComponent->GetOwner());
        return;
    }

    // 선택한 액터가 있다면, 이 컴포넌트가 액터에 붙은 컴포넌트인지 확인
    if (SelectedActor != InSelectedComponent->GetOwner())
    {
        // 아니라면 그 액터 선택
        SelectActor(InSelectedComponent->GetOwner());
        SelectedComponent = InSelectedComponent;
    }
    else
    {
        SelectedComponent = InSelectedComponent;
    }
}

void FEditorManager::ClearSelectedComponent()
{
    SelectedComponent = nullptr;
    SelectedActor = nullptr;
    if (GizmoHandle)
        GizmoHandle->SetActive(false);
}

void FEditorManager::InitializeDefaultViewportCameras(UWorld* World)
{
    if (!World) return;

    TArray<TPair<EEditorViewportType, FVector>> ViewInfos;

    ViewInfos.Add(TPair<EEditorViewportType, FVector>(EEditorViewportType::Perspective, FVector(3.f, -2.f, 2.f)));
    ViewInfos.Add(TPair<EEditorViewportType, FVector>(EEditorViewportType::Top,         FVector(0.f, 0.f, 5.f)));
    ViewInfos.Add(TPair<EEditorViewportType, FVector>(EEditorViewportType::Front,       FVector(0.f, -5.f, 0.f)));
    ViewInfos.Add(TPair<EEditorViewportType, FVector>(EEditorViewportType::Right,       FVector(5.f, 0.f, 0.f)));

    for (const TPair<EEditorViewportType, FVector>& Info : ViewInfos)
    {
        if (Info.Key == EEditorViewportType::Perspective)
        {
            ACamera* Camera = World->SpawnActor<ACamera>();
            SetMainCamera(Camera);
            continue;
        }
        ACamera* Camera = World->SpawnActor<ACamera>();
        FTransform Transform;
        Transform.SetPosition(Info.Value);
        Transform.LookAt(FVector::ZeroVector);
        Camera->SetActorTransform(Transform);

        if (Info.Key != EEditorViewportType::Perspective)
        {
            Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
        }

        RegisterViewportCamera(Info.Key, Camera);
    }
}

void FEditorManager::RegisterViewportCamera(EEditorViewportType Type, ACamera* Camera)
{
    ViewportCameras.Add(Type, Camera);
}

ACamera* FEditorManager::GetViewportCamera(EEditorViewportType Type) const
{
    if (ACamera* const* Found = ViewportCameras.Find(Type))
    {
        return *Found;
    }
    return nullptr;
}
void FEditorManager::SaveSplitterLayout(SSplitter* Root)
{
    if (!Root) return;

    FEngineConfig* Config = UEngine::Get().GetEngineConfig();

    Config->UpdateEngineConfig(EEngineConfigValueType::EEC_SplitterVRatio, Root->GetRatio());

    if (SSplitter* Top = dynamic_cast<SSplitter*>(Root->GetChild(0)))
        Config->UpdateEngineConfig(EEngineConfigValueType::EEC_SplitterHTopRatio, Top->GetRatio());

    if (SSplitter* Bottom = dynamic_cast<SSplitter*>(Root->GetChild(1)))
        Config->UpdateEngineConfig(EEngineConfigValueType::EEC_SplitterHBottomRatio, Bottom->GetRatio());

    //Config->SaveAllConfig();
}
