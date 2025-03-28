#include "pch.h" 
#include "UI.h"

#include "Core/HAL/PlatformMemory.h"
#include "URenderer.h"
#include "Debugging/DebugConsole.h"
#include "Static/EditorManager.h"
#include "CoreUObject/World.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Editor/EditorDesigner.h"
#include "Editor/Font/IconDefs.h"
#include "Editor/Font/RawFonts.h"
#include "Editor/Slate/SSplitter.h"
#include "Editor/Viewport/FViewport.h"
#include "Editor/Windows/ConsoleWindow.h"
#include "Engine/GameFrameWork/Actor.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Engine/GameFrameWork/Sphere.h"
#include "Engine/GameFrameWork/Cube.h"
#include "Engine/GameFrameWork/Arrow.h"
#include "Engine/GameFrameWork/Cone.h"
#include "Engine/GameFrameWork/Cylinder.h"
#include "Gizmo/GizmoHandle.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
#include "Input/PlayerController.h"
#include "Input/PlayerInput.h"
#include "GameFrameWork/AStaticMesh.h"
//@TODO: Replace with EditorWindow

std::shared_ptr<ConsoleWindow> UI::ConsoleWindowInstance = nullptr;
bool UI::bShowStatMemory;
bool UI::bShowStatFPS;

void UI::Initialize(HWND hWnd, const URenderer& Renderer, uint32 InClientWidth, uint32 InClientHeight)
{
	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	CreateUsingFont();

	// Fix Font Size
	io.FontGlobalScale = 1.0f;

	// Initialize ImGui Backend
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(Renderer.GetDevice(), Renderer.GetDeviceContext());

	ClientSize = ImVec2(static_cast<float>(InClientWidth), static_cast<float>(InClientHeight));
	InitialClientSize = ClientSize;
	bIsInitialized = true;

	io.DisplaySize = ClientSize;

	PreRatio = GetRatio();
	CurRatio = GetRatio();

	// Add Windows
	//@TODO: Control, Property, Stat, etc...
	ConsoleWindowInstance = std::make_shared<ConsoleWindow>();
	UEditorDesigner::Get().AddWindow("ConsoleWindow", ConsoleWindowInstance);
}

void UI::Update()
{
	// Set ImGui Style //
	PreferenceStyle();

	// New Frame //
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 화면비 갱신 //
	if (bWasWindowSizeUpdated)
	{
		PreRatio = CurRatio;
		CurRatio = GetRatio();
		UE_LOG("Current Ratio: %f, %f", CurRatio.x, CurRatio.y);
	}

	if (bShowDemoWindow)
		ImGui::ShowDemoWindow(&bShowDemoWindow);

	RenderControlPanelWindow();
	RenderPropertyWindow();
	Debug::ShowConsole(bWasWindowSizeUpdated, PreRatio, CurRatio);
	RenderSceneManagerWindow();
	RenderOverlayStatWindow();
	RenderFPSStat();
	RenderViewportTestWindow();

	// UI::RenderSomePanel 들에 대한 업데이트 완료 //
	bWasWindowSizeUpdated = false;

	// Render Windows //
	UEditorDesigner::Get().Render();

	// Render ImGui //
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	bool bUiInput = ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
	APlayerController::Get().SetIsUiInput(bUiInput);
}


void UI::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void UI::OnClientSizeUpdated(uint32 InClientWidth, uint32 InClientHeight)
{
	// Create ImGUI Resources Again
	ImGui_ImplDX11_InvalidateDeviceObjects();
	ImGui_ImplDX11_CreateDeviceObjects();

	// Resize ImGui Window
	ClientSize = ImVec2(static_cast<float>(InClientWidth), static_cast<float>(InClientHeight));

	bWasWindowSizeUpdated = true;

	// Render Windows //
	UEditorDesigner::Get().OnResize(InClientWidth, InClientHeight);
}

void UI::RenderControlPanelWindow()
{
	ImGui::Begin("Jungle Control Panel");

	if (bWasWindowSizeUpdated)
	{
		auto* Window = ImGui::GetCurrentWindow();

		ImGui::SetWindowPos(ResizeToScreen(Window->Pos));
		ImGui::SetWindowSize(ResizeToScreen(Window->Size));
	}

	ImGui::Text("Hello, Jungle World!");

	ImGui::Separator();

	ImGui::Text("Mouse pos: (%g, %g)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
	ImGui::Text("Screen Size: (%g, %g)", ClientSize.x, ClientSize.y);

	ImGui::Separator();

	//ImGui::Text("FPS: %.3f (what is that ms)", ImGui::GetIO().Framerate);
	//RenderMemoryUsage();

	ImGui::Separator();

	RenderPrimitiveSelection();

	ImGui::Separator();

	RenderCameraSettings();

	ImGui::Separator();

	RenderRenderMode();

	ImGui::Separator();

	RenderGridGap();

	ImGui::Separator();

	RenderDebugRaycast();

	ImGui::Separator();

	if (ImGui::Button("Toggle New Console"))
	{
		UEditorDesigner::Get().Toggle();
	}
	ImGui::SameLine();
	ImGui::Checkbox("Demo Window", &bShowDemoWindow);

	ImGui::End();
}

void UI::RenderMemoryUsage()
{
	const uint64 ContainerAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Container>();
	const uint64 ContainerAllocCount = FPlatformMemory::GetAllocationCount<EAT_Container>();
	const uint64 ObjectAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Object>();
	const uint64 ObjectAllocCount = FPlatformMemory::GetAllocationCount<EAT_Object>();
	ImGui::Text(
		"Container Memory Uses: %llubyte, Count: %llu",
		ContainerAllocByte,
		ContainerAllocCount
	);
	ImGui::Text(
		"Object Memory Uses: %llubyte, Count: %llu Objects",
		ObjectAllocByte,
		ObjectAllocCount
	);
	ImGui::Text(
		"Total Memory Uses: %llubyte, Count: %llu",
		ContainerAllocByte + ObjectAllocByte,
		ContainerAllocCount + ObjectAllocCount
	);
}

void UI::RenderPrimitiveSelection()
{
	const char* items[] = { "Sphere", "Cube", "Cylinder", "Cone", "Arrow", "StaticMesh" };

	ImGui::Combo("Primitive", &currentItem, items, IM_ARRAYSIZE(items));

	if (ImGui::Button("Spawn"))
	{
		UWorld* World = UEngine::Get().GetWorld();
		for (int i = 0; i < NumOfSpawn; i++)
		{
			if (strcmp(items[currentItem], "Sphere") == 0)
			{
				World->SpawnActor<ASphere>();
			}
			else if (strcmp(items[currentItem], "Cube") == 0)
			{
				World->SpawnActor<ACube>();
			}
			else if (strcmp(items[currentItem], "Cylinder") == 0)
			{
				World->SpawnActor<ACylinder>();
			}
			else if (strcmp(items[currentItem], "Cone") == 0)
			{
				World->SpawnActor<ACone>();
			}
			else if (strcmp(items[currentItem], "Arrow") == 0)
			{
				World->SpawnActor<AArrow>();
			}
			else if (strcmp(items[currentItem], "StaticMesh") == 0)
			{
				if (AActor* Actor = World->SpawnActor<AStaticMesh>())
				{
					if (AStaticMesh* MeshActor = dynamic_cast<AStaticMesh*>(Actor))
					{
						MeshActor->InitStaticMeshBoundingBox();
					}
				}
			}
		}
	}
	ImGui::SameLine();
	ImGui::InputInt("Number of spawn", &NumOfSpawn, 0);

	ImGui::Separator();

	UWorld* World = UEngine::Get().GetWorld();
	uint32 bufferSize = 100;
	char* SceneNameInput = new char[bufferSize];

	strcpy_s(SceneNameInput, bufferSize, World->SceneName.c_char());

	if (ImGui::InputText("Scene Name", SceneNameInput, bufferSize))
	{
		World->SceneName = SceneNameInput;
	}

	if (ImGui::Button("New Scene"))
	{
		World->ClearWorld();
	}
	ImGui::SameLine();
	if (ImGui::Button("Save Scene"))
	{
		World->SaveWorld();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Scene"))
	{
		World->LoadWorld(SceneNameInput);
	}
}

void UI::RenderCameraSettings()
{
	ImGui::Text("Camera");

    ACamera* Camera = FEditorManager::Get().GetMainCamera();

	bool IsOrthogonal;
	if (Camera->GetProjectionMode() == ECameraProjectionMode::Orthographic)
	{
		IsOrthogonal = true;
	}
	else if (Camera->GetProjectionMode() == ECameraProjectionMode::Perspective)
	{
		IsOrthogonal = false;
	}

	if (ImGui::Checkbox("Orthogonal", &IsOrthogonal))
	{
		if (IsOrthogonal)
		{
			Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
		}
		else
		{
			Camera->SetProjectionMode(ECameraProjectionMode::Perspective);
		}
	}

	float FOV = Camera->GetFieldOfView();
	if (ImGui::DragFloat("FOV", &FOV, 0.1f, 20.f, 150.f))
	{
		FOV = FMath::Clamp(FOV, 20.f, 150.f);
		Camera->SetFieldOfView(FOV);
	}

	float NearFar[2] = { Camera->GetNearClip(), Camera->GetFarClip() };
	if (ImGui::DragFloat2("Near clip, Far clip", NearFar, 0.1f, 0.01f, 200.f))
	{
		NearFar[0] = FMath::Clamp(NearFar[0], 0.01f, 200.f);
		NearFar[1] = FMath::Clamp(NearFar[1], 0.01f, 200.f);

		if (NearFar[0] > NearFar[1])
		{
			std::swap(NearFar[0], NearFar[1]);
		}

		Camera->SetNear(NearFar[0]);
		Camera->SetFar(NearFar[1]);
	}

	FVector CameraPosition = Camera->GetActorTransform().GetLocation();
	if (ImGui::DragFloat3("Camera Location", reinterpret_cast<float*>(&CameraPosition), 0.1f))
	{
		FTransform Trans = Camera->GetActorTransform();
		Trans.SetPosition(CameraPosition);
		Camera->SetActorTransform(Trans);
	}

	FVector PrevEulerAngle = Camera->GetActorTransform().GetRotation().GetEuler();
	FVector UIEulerAngle = { PrevEulerAngle.X, PrevEulerAngle.Y, PrevEulerAngle.Z };
	if (ImGui::DragFloat3("Camera Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
	{
		FTransform Transform = Camera->GetActorTransform();

		UIEulerAngle.Y = FMath::Clamp(UIEulerAngle.Y, -Camera->GetMaxPitch(), Camera->GetMaxPitch());
		Transform.SetRotation(UIEulerAngle);
		Camera->SetActorTransform(Transform);
	}

	float CurrentSpeed = APlayerController::Get().GetCurrentSpeed();
	const float CameraMaxSpeed = APlayerController::Get().GetMaxSpeed();
	const float CameraMinSpeed = APlayerController::Get().GetMinSpeed();
	if (ImGui::DragFloat("Camera Speed", &CurrentSpeed, 0.1f, CameraMinSpeed, CameraMaxSpeed))
	{
		APlayerController::Get().SetCurrentSpeed(CurrentSpeed);
	}

	float CurrentSensitivity = APlayerController::Get().GetMouseSensitivity();
	const float CameraMaxSensitivity = APlayerController::Get().GetMaxSensitivity();
	const float CameraMinSensitivity = APlayerController::Get().GetMinSensitivity();
	if (ImGui::DragFloat("Camera Sensitivity", &CurrentSensitivity, 0.01f, CameraMinSensitivity, CameraMaxSensitivity))
	{
		APlayerController::Get().SetMouseSensitivity(CurrentSensitivity);
	}

	FVector Forward = Camera->GetActorTransform().GetForward();
	FVector Up = Camera->GetActorTransform().GetUp();
	FVector Right = Camera->GetActorTransform().GetRight();

	ImGui::Text("Camera GetForward(): (%.2f %.2f %.2f)", Forward.X, Forward.Y, Forward.Z);
	ImGui::Text("Camera GetUp(): (%.2f %.2f %.2f)", Up.X, Up.Y, Up.Z);
	ImGui::Text("Camera GetRight(): (%.2f %.2f %.2f)", Right.X, Right.Y, Right.Z);

	ImGui::Separator();
}

void UI::RenderRenderMode()
{
	const char* items[] = { "Solid", "Wireframe" };
	ImGui::Combo("Render Mode", &currentItem, items, IM_ARRAYSIZE(items));
	if (ImGui::Button("Apply"))
	{
		URenderer* Renderer = UEngine::Get().GetRenderer();
		if (currentItem == 0)
		{
			Renderer->SetRenderMode(EViewModeIndex::ERS_Solid);
		}
		else if (currentItem == 1)
		{
			Renderer->SetRenderMode(EViewModeIndex::ERS_Wireframe);
		}
	}

	bool bShowPrimitives = UEngine::Get().GetShowPrimitives();
	if (ImGui::Checkbox("Show Primitives", &bShowPrimitives))
	{
		UEngine::Get().SetShowPrimitives(bShowPrimitives);
	}

	ImGui::Separator();
}

void UI::RenderPropertyWindow()
{
	ImGui::Begin("Properties");

	if (bWasWindowSizeUpdated)
	{
		auto* Window = ImGui::GetCurrentWindow();

		ImGui::SetWindowPos(ResizeToScreen(Window->Pos));
		ImGui::SetWindowSize(ResizeToScreen(Window->Size));
	}
	USceneComponent* SelectedComponent = FEditorManager::Get().GetSelectedComponent();
	if (SelectedComponent != nullptr)
	{
		ImGui::Text("Selected Actor : %s", SelectedComponent->GetOwner()->GetName().c_char());
		ImGui::Text("Selected Component : %s", SelectedComponent->GetName().c_char());

		bool bIsLocal = FEditorManager::Get().GetGizmoHandle()->bIsLocal;
		if (ImGui::Checkbox("Local", &bIsLocal))
		{
			FEditorManager::Get().ToggleGizmoHandleLocal(bIsLocal);
		}

		FTransform selectedTransform = SelectedComponent->GetComponentTransform();
		float position[] = { selectedTransform.GetLocation().X, selectedTransform.GetLocation().Y, selectedTransform.GetLocation().Z };
		float scale[] = { selectedTransform.GetScale().X, selectedTransform.GetScale().Y, selectedTransform.GetScale().Z };

		if (ImGui::DragFloat3("Translation", position, 0.1f))
		{
			selectedTransform.SetPosition(position[0], position[1], position[2]);
			SelectedComponent->SetRelativeTransform(selectedTransform);
		}

		FVector PrevEulerAngle = selectedTransform.GetRotation().GetEuler();
		FVector UIEulerAngle = PrevEulerAngle;
		if (ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
		{
			FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;

			selectedTransform.Rotate(DeltaEulerAngle);
			SelectedComponent->SetRelativeTransform(selectedTransform);
		}
		if (ImGui::DragFloat3("Scale", scale, 0.1f))
		{
			selectedTransform.SetScale(scale[0], scale[1], scale[2]);
			SelectedComponent->SetRelativeTransform(selectedTransform);
		}
		if (FEditorManager::Get().GetGizmoHandle() != nullptr)
		{
			AGizmoHandle* Gizmo = FEditorManager::Get().GetGizmoHandle();
			if (Gizmo->GetGizmoType() == EGizmoType::Translate)
			{
				ImGui::Text("GizmoType: Translate");
			}
			else if (Gizmo->GetGizmoType() == EGizmoType::Rotate)
			{
				ImGui::Text("GizmoType: Rotate");
			}
			else if (Gizmo->GetGizmoType() == EGizmoType::Scale)
			{
				ImGui::Text("GizmoType: Scale");
			}
		}

		if (ImGui::Button("Remove"))
		{
			UWorld* World = UEngine::Get().GetWorld();
			//FEditorManager::Get().SelectComponent(nullptr);
			World->DestroyActor(SelectedComponent->GetOwner());
		}

		ImGui::Separator();
		const char* Items[] = { "BT.obj","x-35_obj.obj", "cube.obj", "pineapple.obj", "mst.obj", "plant.obj","Cat.obj", "tent.obj","raccoon.obj","pubg.obj","bshobj.obj","icg.obj","ksw.obj","giant.obj" };

		if (SelectedComponent != PrevSelectedComponent)
		{
			AStaticMesh* StaticMeshOwner = dynamic_cast<AStaticMesh*>(SelectedComponent->GetOwner());
			if (StaticMeshOwner)
			{
				FString MeshName = StaticMeshOwner->GetAssetName();
				for (int i = 0; i < IM_ARRAYSIZE(Items); ++i)
				{
					if (MeshName.Equals(Items[i]))
					{
						CurrentMesh = i;
						break;
					}
				}
			}
			PrevSelectedComponent = SelectedComponent;
		}

		if (ImGui::BeginCombo("Static Mesh DropList", Items[CurrentMesh]))
		{
			for (int i = 0; i < IM_ARRAYSIZE(Items); i++)
			{
				bool isSelected = (CurrentMesh == i);
				if (ImGui::Selectable(Items[i], isSelected)) {
					CurrentMesh = i;
					AStaticMesh* SelectedActor = dynamic_cast<AStaticMesh*>(SelectedComponent->GetOwner());
					if (UStaticMeshComponent* MeshComponent = SelectedActor->FindComponent<UStaticMeshComponent>())
					{
						SelectedActor->SetAssetName(Items[CurrentMesh]);
						MeshComponent->ChangeStaticMesh(Items[CurrentMesh]);
					}
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	ImGui::End();
}

void UI::RenderFPSStat()
{
	if (!bShowStatFPS) return;
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	// FPS 창 크기는 자동조절되므로, 윈도우 크기만큼 오프셋을 주려면 추정치를 사용할 수도 있지만
	// 간단히 화면 우측 중앙에서 약간 안쪽으로 위치 조정
	float offsetX = 10.0f; // 화면 오른쪽에서 10픽셀 안쪽
	float posX = viewport->Pos.x + viewport->Size.x - offsetX;
	float posY = viewport->Pos.y + viewport->Size.y * 0.5f;

	ImGui::SetNextWindowPos(ImVec2(posX, posY), ImGuiCond_Always, ImVec2(1.0f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));


	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoInputs;

	ImGui::Begin("FPSOverlay", nullptr, windowFlags);
	ImGui::SetWindowFontScale(1.5f);
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "FPS: %.1f (%.1f ms)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::SetWindowFontScale(1.0f);

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}

void UI::RenderGridGap()
{
	ImGui::Text("World Grid");

	float MaxVal = 10.f;
	float MinVal = 0.5f;

	float GridGap = UEngine::Get().GetWorldGridGap();

	if (ImGui::DragFloat("Grid Gap", &GridGap, 0.01f, MinVal, MaxVal))
	{
		GridGap = GridGap > MaxVal ? MaxVal : (GridGap < MinVal ? MinVal : GridGap); // Clamp
		UEngine::Get().SetWorldGridGap(GridGap);
	}

	ImGui::Separator();
}

void UI::RenderDebugRaycast()
{
	bool bDebugRaycast = UEngine::Get().GetWorld()->IsDebuggingRaycast();
	if (ImGui::Checkbox("Debug Raycast", &bDebugRaycast))
	{
		UEngine::Get().GetWorld()->SetDebugRaycast(bDebugRaycast);
	}
}

void UI::RenderSceneManagerWindow()
{
	// Using those as a base value to create width/height that are factor of the size of our font
	const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
	const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

	ImGui::Begin("Outliner");
	/*
	TArray<AActor*> Actors = UEngine::Get().GetWorld()->GetActors();

	if (ImGui::TreeNodeEx("Primitives", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (const auto& Actor : Actors)
		{
			if (Actor->IsGizmoActor() || Actor->IsA<AAxis>())
			{
				continue;
			}

			TSet<UActorComponent*> Comps = Actor->GetComponents();

			bool bHasPrimitive = false;
			for (const auto& Comp : Comps)
			{
				if (Comp->IsA<UPrimitiveComponent>())
				{
					bHasPrimitive = true;
					break;
				}
			}

			if (bHasPrimitive)
			{
				ImGui::Text(*Actor->GetName());
			}
		}

		ImGui::TreePop();
	}
	*/

	static ImGuiSelectionBasicStorage OutlinerSelection;
	ImGui::Text("Selection size: %d", OutlinerSelection.Size);
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBodyUntilResize | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable;

	if (ImGui::BeginTable("table", 7, flags, ImVec2(0.0f, TEXT_BASE_HEIGHT * 16)))
	{
		ImGui::TableSetupColumn("V", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_IndentDisable);
		ImGui::TableSetupColumn("*", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
		ImGui::TableSetupColumn("P", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize);
		ImGui::TableSetupColumn("ItemLabel", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
		ImGui::TableSetupColumn("UUID", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
		ImGui::TableSetupColumn("PUUID", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
		ImGui::TableHeadersRow();

		ActorTreeNode* tree = UEngine::Get().GetWorld()->WorldNode;
		ImGuiMultiSelectFlags ms_flags = ImGuiMultiSelectFlags_ClearOnEscape | ImGuiMultiSelectFlags_BoxSelect2d;
		ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(ms_flags, OutlinerSelection.Size, -1);
		ActorTreeNode::ApplySelectionRequests(ms_io, tree, &OutlinerSelection);
		ActorTreeNode::DisplayNode(tree, &OutlinerSelection);
		ms_io = ImGui::EndMultiSelect();
		ActorTreeNode::ApplySelectionRequests(ms_io, tree, &OutlinerSelection);

		ImGui::EndTable();
	}
	ImGui::End();
}

void UI::RenderOverlayStatWindow()
{
	if (!bShowStatMemory) return;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoInputs;

	// 스타일 적용 (테두리 제거 + 완전 투명 배경)
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

	ImGui::Begin("MemoryTableOverlay", nullptr, windowFlags);

	// 폰트 스케일 키우기 (1.5배)
	ImGui::SetWindowFontScale(1.5f);

	// 셀 패딩 최소화, 열은 자동 너비 조정
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(6, 4));

	ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;

	if (ImGui::BeginTable("MemoryTable", 3, tableFlags))
	{
		ImGui::TableSetupColumn("Memory Type");
		ImGui::TableSetupColumn("Allocated Memory (MB)");
		ImGui::TableSetupColumn("Allocation Count");

		// Header row
		const char* headers[3] = { "Memory Type", "Allocated Memory (MB)", "Allocation Count" };
		ImGui::TableNextRow();
		for (int col = 0; col < 3; col++)
		{
			ImGui::TableSetColumnIndex(col);
			ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "%s", headers[col]);
		}

		// 메모리 데이터 가져오기
		const uint64 containerBytes = FPlatformMemory::GetAllocationBytes<EAT_Container>();
		const uint64 containerCount = FPlatformMemory::GetAllocationCount<EAT_Container>();
		const uint64 objectBytes = FPlatformMemory::GetAllocationBytes<EAT_Object>();
		const uint64 objectCount = FPlatformMemory::GetAllocationCount<EAT_Object>();

		double containerMB = static_cast<double>(containerBytes) / (1024.0 * 1024.0);
		double objectMB = static_cast<double>(objectBytes) / (1024.0 * 1024.0);

		// 표 데이터 구성
		TArray<TArray<std::string>> rows;
		{
			TArray<std::string> row;
			row.Add("Container");
			row.Add(std::to_string(containerMB));
			row.Add(std::to_string(containerCount));
			rows.Add(row);
		}
		{
			TArray<std::string> row;
			row.Add("Object");
			row.Add(std::to_string(objectMB));
			row.Add(std::to_string(objectCount));
			rows.Add(row);
		}
		{
			TArray<std::string> row;
			double totalMB = containerMB + objectMB;
			uint64 totalCount = containerCount + objectCount;
			row.Add("TotalAllocated");
			row.Add(std::to_string(totalMB));
			row.Add(std::to_string(totalCount));
			rows.Add(row);
		}

		// 표 렌더링
		for (const auto& row : rows)
		{
			ImGui::TableNextRow();
			ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0, 0, 0, 160));
			for (int col = 0; col < row.Num(); col++)
			{
				ImGui::TableSetColumnIndex(col);
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", row[col].c_str());
			}
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar(); // CellPadding
	ImGui::SetWindowFontScale(1.0f);
	ImGui::End();
	ImGui::PopStyleColor(); // WindowBg
	ImGui::PopStyleVar();   // WindowBorderSize
}


void UI::PreferenceStyle()
{
	// Window
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.9f);
	ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.5f, 0.0f, 1.0f);
	ImGui::GetStyle().Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	ImGui::GetStyle().WindowRounding = 5.0f;

	ImGui::GetStyle().FrameRounding = 3.0f;

	// Sep
	ImGui::GetStyle().Colors[ImGuiCol_Separator] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

	// Frame
	ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.31f, 0.31f, 0.31f, 0.6f);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBgActive] = ImVec4(0.203f, 0.203f, 0.203f, 0.6f);
	ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

	// Button
	ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
	ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

	ImGui::GetStyle().Colors[ImGuiCol_Header] = ImVec4(0.203f, 0.203f, 0.203f, 0.6f);
	ImGui::GetStyle().Colors[ImGuiCol_HeaderActive] = ImVec4(0.105f, 0.105f, 0.105f, 0.6f);
	ImGui::GetStyle().Colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 0.5f, 0.0f, 0.6f);

	// Text
	ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 0.9f);

}
void UI::RenderViewportTestWindow()
{
	ImGui::Begin("Viewport Splitter");

	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (!Renderer)
	{
		ImGui::End();
		return;
	}

	// Splitter On/Off 토글
	if (ImGui::Checkbox("Use Splitter", &Renderer->bUseSplitter))
	{
		// 체크박스 변경 시 초기화 (뷰포트 구조 재구성)
		Renderer->InitializeViewports();
		if (!Renderer->bUseSplitter)
		{
			FViewport* FView = Renderer->MainViewport->GetFViewport();
			SViewport* SView = Renderer->MainViewport;

			// 1. Splitter 상태를 MainViewport만 남기도록 강제 설정
			SSplitter* RootSplitter = dynamic_cast<SSplitter*>(Renderer->RootWindow);
			if (RootSplitter)
			{
				RootSplitter->SetRatio(0,true); // 전체 Bottom 영역 사용
				if (SSplitter* BottomSplitter = dynamic_cast<SSplitter*>(RootSplitter->GetChild(1)))
				{
					BottomSplitter->SetRatio(1,true); // 전체 Left 영역 사용
				}
				RootSplitter->UpdateChildRects(); // Rect 재계산
			}

			// 2. 전체 화면 기준으로 SViewport Rect 설정
			FRect FullRect(0, 0, Renderer->ViewportInfo.Width, Renderer->ViewportInfo.Height);
			SView->SetRect(FullRect);

			// 3. FViewport의 사이즈 동기화
			FView->Resize(FullRect.Width, FullRect.Height);
		}
		Renderer->ResizeViewports();
	}

	ImGui::Checkbox("Render Picking", &Renderer->bRenderPicking);

	ImGui::End();
}

void UI::CreateUsingFont()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\malgun.ttf)", 14.0f, NULL, io.Fonts->GetGlyphRangesKorean());

	ImFontConfig FeatherFontConfig;
	FeatherFontConfig.PixelSnapH = true;
	FeatherFontConfig.FontDataOwnedByAtlas = false;
	FeatherFontConfig.GlyphOffset = ImVec2(0, 0);
	static constexpr ImWchar IconRanges[] = {
		ICON_MOVE,      ICON_MOVE + 1,
		ICON_ROTATE,    ICON_ROTATE + 1,
		ICON_SCALE,     ICON_SCALE + 1,
		ICON_MONITOR,   ICON_MONITOR + 1,
		ICON_BAR_GRAPH, ICON_BAR_GRAPH + 1,
		ICON_NEW,       ICON_NEW + 1,
		ICON_SAVE,      ICON_SAVE + 1,
		ICON_LOAD,      ICON_LOAD + 1,
		0 };

	io.Fonts->AddFontFromMemoryTTF(FeatherRawData, FontSizeOfFeather, 22.0f, &FeatherFontConfig, IconRanges);
}