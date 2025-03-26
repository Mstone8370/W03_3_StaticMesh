#pragma once

#include "Engine/GameFrameWork/Actor.h"

class AGizmoHandle;

enum class EEditorViewportType : uint8
{
    Front,
    Right,
    Perspective,
    Top,
    MAX
};

class FEditorManager : public TSingleton<FEditorManager>
{
public:
    //inline AActor* GetSelectedActor() const {return SelectedActor;}

    void SelectActor(AActor* NewActor);

    inline ACamera* GetMainCamera() const { return MainCamera; }

    void SetMainCamera(ACamera* NewCamera);

    bool IsMainCamera(const ACamera* InCamera) const
    {
        return MainCamera == InCamera;
    }

    void SetGizmoHandle(AGizmoHandle* NewGizmoHandle) { GizmoHandle = NewGizmoHandle; }
    AGizmoHandle* GetGizmoHandle() const { return GizmoHandle; }

    void ToggleGizmoHandleLocal(bool bIsLocal);

    USceneComponent* GetSelectedComponent() const;

    void SelectComponent(USceneComponent* SelectedComponent);

    void ClearSelectedComponent();
    void InitializeDefaultViewportCameras(UWorld* World);
    void RegisterViewportCamera(EEditorViewportType Type, ACamera* Camera);
    ACamera* GetViewportCamera(EEditorViewportType Type) const;
    void SaveSplitterLayout(SSplitter* Root);

private:
    ACamera* MainCamera = nullptr;
    AActor* SelectedActor = nullptr;
    USceneComponent* SelectedComponent;
    AGizmoHandle* GizmoHandle = nullptr;
    TMap<EEditorViewportType, ACamera*> ViewportCameras;
};
