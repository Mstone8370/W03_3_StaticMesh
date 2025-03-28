#pragma once

#define _TCHAR_DEFINED

// ImGui include
#include "ImGui/imgui.h"

class AActor;
class URenderer;
class ConsoleWindow;
class USceneComponent;

class UI
{
public:
	int currentItem = 0;
	int NumOfSpawn = 1;
	bool bIsInitialized = false;
	static bool bShowStatMemory;
	static bool bShowStatFPS;
	
	void Initialize(HWND hWnd, const class URenderer& Renderer, UINT InClientWidth, UINT InClientHeight);
	void Update();
	void Shutdown();

	void OnClientSizeUpdated(uint32 InClientWidth, uint32 InClientHeight);

public:// UIWindows
	void RenderControlPanelWindow();
	void RenderMemoryUsage();
	void RenderPrimitiveSelection();
	void RenderCameraSettings();
	void RenderRenderMode();
	void RenderPropertyWindow();
	void RenderFPSStat();
	void RenderGridGap();
	void RenderDebugRaycast();
	void RenderSceneManagerWindow();
	void RenderOverlayStatWindow();
	void RenderViewportTestWindow();

private:
	void PreferenceStyle();

	void CreateUsingFont();

	// Mouse 전용
	ImVec2 ResizeToScreenByCurrentRatio(const ImVec2& vec2) const
	{
		return {vec2.x / CurRatio.x, vec2.y / CurRatio.y };
	}
	
	ImVec2 ResizeToScreen(const ImVec2& vec2) const
	{
		float ratio = GetMin();
		float preMin = GetPreMin();
		return {vec2.x * PreRatio.x / CurRatio.x * ratio / preMin, vec2.y * PreRatio.y / CurRatio.y * ratio / preMin};
	}

	ImVec2 GetRatio() const
	{
		return {ClientSize.x / InitialClientSize.x, ClientSize.y / InitialClientSize.y};
	}

	float GetMin() const
	{
		if (CurRatio.x < CurRatio.y)
		{
			return CurRatio.x;
		}
		else
		{
			return CurRatio.y;
		}
	}

	float GetPreMin() const
	{
		if (PreRatio.x < PreRatio.y)
		{
			return PreRatio.x;
		}
		else
		{
			return PreRatio.y;
		}
	}

	// UI::RenderSomePanel()에서 화면 갱신을 알기 위한 함수
	bool bWasWindowSizeUpdated = false;

	//note: imgui_demo.cpp의 ImGui::ShowDemoWindow를 위한 불리언
	bool bShowDemoWindow = false;
	
	ImVec2 ClientSize;
	ImVec2 InitialClientSize;

	ImVec2 PreRatio;
	ImVec2 CurRatio;

	USceneComponent* PrevSelectedComponent = nullptr;
	int CurrentMesh = 0;
public:
	static std::shared_ptr<ConsoleWindow> ConsoleWindowInstance;
};
