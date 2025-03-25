#pragma once

#include "Engine/GameFrameWork/Actor.h"
#include "SlateCore/SWindow.h"

class AGizmoHandle;

enum class EViewportMode
{
	EVM_Single,
	EVM_Quad,

	EVM_Max,
};

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

	void GetAllViewportWidgets(TArray<SViewport*>& OutViewports) const;

	SViewport* GetActiveViewport(const FPoint& Point) const;

	void ActivateQuadViewport();

	void DeactivateQuadViewport();

	bool IsQuadViewportActive() const { return ViewportMode == EViewportMode::EVM_Quad; }

	void OnResize(uint32 InWidth, uint32 InHeight);

	FVector GetOrthogonalPoint() const { return OrthogonalPoint; }
    
private:
	// 메인 에디터 윈도우
	std::unique_ptr<SWindow> EditorWindow = nullptr;

	EViewportMode ViewportMode;

	FRect MainRect;

	FVector OrthogonalPoint = FVector::ZeroVector;
	
    ACamera* Camera = nullptr;
    AActor* SelectedActor = nullptr;
    USceneComponent* SelectedComponent = nullptr;
    AGizmoHandle* GizmoHandle = nullptr;

	void TraverseViewports(SWindow* CurrentWindow, TArray<SViewport*>& OutViewports) const;

	SViewport* FindActiveViewport(SWindow* CurrentWindow, const FPoint& Point) const;

	// Release RTV, DSV
	void ReleaseAllViewports();

	// Re-assign RTV, DSV with resized width and height
	void ResizeViewports(uint32 InWidth, uint32 InHeight);
};
