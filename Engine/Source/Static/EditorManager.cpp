#include "pch.h" 
#include "EditorManager.h"
#include "Engine/Engine.h"
#include "CoreUObject/World.h"
#include "Gizmo/GizmoHandle.h"

void FEditorManager::Init(int32 InWidth, int32 InHeight)
{
    ViewportMode = EViewportMode::EVM_Single;
    
    if (EditorWindow)
    {
        EditorWindow.reset();
    }
    
    EditorWindow = std::make_unique<SWindow>();
    MainRect = FRect(0.f, 0.f, InWidth, InHeight);
    EditorWindow->Init(MainRect);

    ActivateQuadViewport();
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

void FEditorManager::GetAllViewportWidgets(TArray<SViewport*>& OutViewports) const
{
    OutViewports.Empty();
    TraverseViewports(EditorWindow.get(), OutViewports);
}

SViewport* FEditorManager::GetActiveViewport(const FPoint& Point) const
{
    return FindActiveViewport(EditorWindow.get(), Point);   
}

void FEditorManager::ActivateQuadViewport()
{
    if (ViewportMode == EViewportMode::EVM_Quad)
    {
        return;
    }
    ViewportMode = EViewportMode::EVM_Quad;
    
    if (EditorWindow)
    {
        EditorWindow.reset();
    }

    EditorWindow = std::make_unique<SSplitterH>();
    EditorWindow->Init(MainRect);

    // 하드 코딩으로 뷰포트 4개 생성
    if (SSplitterH* Splitter = dynamic_cast<SSplitterH*>(EditorWindow.get()))
    {
        Splitter->Split();
        SWindow* Left = Splitter->SideLT;
        if (SSplitterV* SplitterV = dynamic_cast<SSplitterV*>(Left))
        {
            SplitterV->Split();
        }
        SWindow* Right = Splitter->SideRB;
        if (SSplitterV* SplitterV = dynamic_cast<SSplitterV*>(Right))
        {
            SplitterV->Split();
        }
    }
}

void FEditorManager::DeactivateQuadViewport()
{
    if (ViewportMode == EViewportMode::EVM_Single)
    {
        return;
    }
    ViewportMode = EViewportMode::EVM_Single;
    
    if (EditorWindow)
    {
        EditorWindow.reset();
    }
    
    EditorWindow = std::make_unique<SWindow>();
    EditorWindow->Init(MainRect);
}

void FEditorManager::TraverseViewports(SWindow* CurrentWindow, TArray<SViewport*>& OutViewports) const
{
    if (CurrentWindow == nullptr)
    {
        return;
    }

    if (SSplitter* Splitter = dynamic_cast<SSplitter*>(CurrentWindow))
    {
        TraverseViewports(Splitter->SideLT, OutViewports);
        TraverseViewports(Splitter->SideRB, OutViewports);
    }
    else if (SViewport* Viewport = CurrentWindow->Viewport.get())
    {
        OutViewports.Add(Viewport);
    }
}

SViewport* FEditorManager::FindActiveViewport(SWindow* CurrentWindow, const FPoint& Point) const
{
    if (CurrentWindow == nullptr)
    {
        return nullptr;
    }

    if (SSplitter* Splitter = dynamic_cast<SSplitter*>(CurrentWindow))
    {
        if (SViewport* Viewport = FindActiveViewport(Splitter->SideLT, Point))
        {
            return Viewport;
        }
        return FindActiveViewport(Splitter->SideRB, Point);
    }
    
    if (SViewport* Viewport = CurrentWindow->Viewport.get())
    {
        if (Viewport->IsHover(Point))
        {
            return Viewport;
        }
    }
    
    return nullptr;
}
