#pragma once

#include "Engine/GameFrameWork/Actor.h"

class AGizmoHandle;
enum class EEditorViewportType : uint8
{
	Perspective,
	Top,
	Front,
	Right,
	MAX
};
class FEditorManager : public TSingleton<FEditorManager>
{
public:
    
    //inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return ViewportCameras[EEditorViewportType::Perspective];}

    void SetCamera(ACamera* NewCamera);

	void SetGizmoHandle(AGizmoHandle* NewGizmoHandle) { GizmoHandle = NewGizmoHandle; }
    AGizmoHandle* GetGizmoHandle() const {return GizmoHandle;}

    void ToggleGizmoHandleLocal(bool bIsLocal);

    USceneComponent* GetSelectedComponent() const;

	void SelectComponent(USceneComponent* SelectedComponent);

	void ClearSelectedComponent();
	void InitializeDefaultViewportCameras(UWorld* World);
	void RegisterViewportCamera(EEditorViewportType Type, ACamera* Camera);
	ACamera* GetViewportCamera(EEditorViewportType Type) const;

private:
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    USceneComponent* SelectedComponent;
    AGizmoHandle* GizmoHandle = nullptr;
	TMap<EEditorViewportType, ACamera*> ViewportCameras;
};
