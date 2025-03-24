#include "pch.h" 
#include "EditorManager.h"
#include "Engine/Engine.h"
#include "CoreUObject/World.h"
#include "Gizmo/GizmoHandle.h"

void FEditorManager::Init(int32 InWidth, int32 InHeight)
{
    if (!EditorWindow)
    {
        EditorWindow = std::make_unique<SWindow>();
        FRect Temp(0.f, 0.f, InWidth, InHeight); // Temp
        EditorWindow->Init(Temp);
    }
}

void FEditorManager::Tick(const float DeltaTime)
{
    if (EditorWindow)
    {
        EditorWindow->HandleInput(DeltaTime);
        EditorWindow->Tick(DeltaTime);
        EditorWindow->Paint();
    }
}

void FEditorManager::SelectActor(AActor* NewActor)
{
    if (NewActor == nullptr)
    {
        return;
    }

    if (GizmoHandle == nullptr)
    {
        GizmoHandle = UEngine::Get().GetWorld()->SpawnActor<AGizmoHandle>();
        GizmoHandle->SetActive(false);
    }

    if(SelectedActor != nullptr)
        SelectedActor->UnPick();

    SelectedActor = NewActor;
	SelectedActor->Pick();
	GizmoHandle->SetActive(true);
}

void FEditorManager::SetCamera(ACamera* NewCamera)
{
    Camera = NewCamera;
}

void FEditorManager::ToggleGizmoHandleLocal(bool bIsLocal)
{
    if (GizmoHandle)
        GizmoHandle->SetLocal(bIsLocal);
}

USceneComponent* FEditorManager::GetSelectedComponent() const
{
    if(SelectedComponent == nullptr)
	{
        if(SelectedActor)
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
    if(GizmoHandle)
        GizmoHandle->SetActive(false);
}

void FEditorManager::HandleInput(const float DeltaTime)
{
    if (EditorWindow)
    {
        EditorWindow->HandleInput(DeltaTime);
    }
}

void FEditorManager::GetAllViewports(TArray<FViewport*>& OutViewports) const
{
    OutViewports.Empty();
    // TODO: 트리 순환하면서 모두 찾기. 지금은 하드코딩
    OutViewports.Add(EditorWindow.get()->Viewport.get()->Viewport.get());
}
