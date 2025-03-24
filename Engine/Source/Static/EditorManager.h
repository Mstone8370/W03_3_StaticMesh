#pragma once

#include "Engine/GameFrameWork/Actor.h"
#include "SlateCore/SWindow.h"

class AGizmoHandle;

class FEditorManager : public TSingleton<FEditorManager>
{
public:
	void Init(int32 InWidth, int32 InHeight);

	virtual void Tick(const float DeltaTime);
    
    //inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return Camera;}

    void SetCamera(ACamera* NewCamera);

	void SetGizmoHandle(AGizmoHandle* NewGizmoHandle) { GizmoHandle = NewGizmoHandle; }
    AGizmoHandle* GetGizmoHandle() const {return GizmoHandle;}

    void ToggleGizmoHandleLocal(bool bIsLocal);

    USceneComponent* GetSelectedComponent() const;

	void SelectComponent(USceneComponent* SelectedComponent);

	void ClearSelectedComponent();

	void HandleInput(const float DeltaTime);

	void GetAllViewports(TArray<FViewport*>& OutViewports) const;
    
private:
	// 메인 에디터 윈도우
	std::unique_ptr<SWindow> EditorWindow = nullptr;
	
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    USceneComponent* SelectedComponent = nullptr;
    AGizmoHandle* GizmoHandle = nullptr;
};
