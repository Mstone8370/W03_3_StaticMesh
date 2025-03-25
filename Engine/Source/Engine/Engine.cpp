#include "pch.h" 
#include "Engine.h"

#include "Static/EditorManager.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Input/PlayerController.h"
#include "CoreUObject/ObjectFactory.h"
#include "CoreUObject/World.h"
#include "Gizmo/Axis.h"
#include "GameFrameWork/Camera.h"
#include "Gizmo/GizmoHandle.h"
#include "Core/Rendering/TextureLoader.h"
#include "GameFrameWork/Picker.h"

#include "Delegates/Delegate.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTK/Libs/x64/Debug/DirectXTK.lib")
#else
#pragma comment(lib, "DirectXTK/Libs/x64/Release/DirectXTK.lib")
#endif

// ImGui WndProc
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

UEngine::UEngine()
    : TargetFPS(60)
{}

LRESULT UEngine::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Handle ImGui Msg
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    {
        return true;
    }
    
    switch (uMsg)
    {
    case WM_DESTROY:    // Window Close, Alt + F4
        PostQuitMessage(0);
        return 0;

    // Begin Handle Input
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEWHEEL:
        DirectX::Mouse::ProcessMessage(uMsg, wParam, lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
        DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
        break;
    // End Handle Input
    
    case WM_SIZE:
        {   // 다른 case에서 아래의 변수에 접근하지 못하도록 스코프 제한
            if (wParam == SIZE_MINIMIZED)
            {
                return 0;
            }
            int32 ClientWidth = LOWORD(lParam);
            int32 ClientHeight = HIWORD(lParam);
            UEngine::Get().UpdateWindowSize(ClientWidth, ClientHeight);
        }
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void UEngine::Initialize(HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth, int InScreenHeight, EScreenMode InScreenMode)
{
    // ini파일 로드
	EngineConfig = new FEngineConfig();
	EngineConfig->LoadEngineConfig();

	uint32 ConfigWidth = EngineConfig->GetEngineConfigValue<uint32>(EEngineConfigValueType::EEC_ScreenWidth);
	uint32 ConfigHeight = EngineConfig->GetEngineConfigValue<uint32>(EEngineConfigValueType::EEC_ScreenHeight);

    WindowInstance = hInstance;
    WindowTitle = InWindowTitle;
    WindowClassName = InWindowClassName;
    ClientWidth = ConfigWidth <= 0 ? InScreenWidth : ConfigWidth;
    ClientHeight = ConfigHeight <= 0 ? InScreenHeight : ConfigHeight;

    ScreenMode = InScreenMode;

    InitWindow(ClientWidth, ClientHeight);

    InitRenderer();

    InitTextureLoader();
    
    InitWorld();

    FEditorManager::Get().Init(ClientWidth, ClientHeight);
    
    ui.Initialize(WindowHandle, *Renderer, ClientWidth, ClientHeight);

    UpdateWindowSize(ClientWidth, ClientHeight);
    
    UE_LOG("Engine Initialized!");
    
#ifdef _DEBUG
    World->LoadWorld("Default");
#endif

}

void UEngine::Run()
{
    double TargetDeltaTime = -1.f;
    
    bool bShouldLimitFPS = TargetFPS > 0;
    if (bShouldLimitFPS)
    {
        // Limit FPS
        TargetDeltaTime = 1000.0f / static_cast<double>(TargetFPS); // 1 FPS's target time (ms)
    }

    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime;
    QueryPerformanceCounter(&StartTime);

    bIsExit = true;
    while (bIsExit)
    {
        // DeltaTime //
        const LARGE_INTEGER EndTime = StartTime;
        QueryPerformanceCounter(&StartTime);

        const float DeltaTime = static_cast<float>(StartTime.QuadPart - EndTime.QuadPart) / static_cast<float>(Frequency.QuadPart);

		// Message Loop //
        MSG Msg;
        while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Msg);
            DispatchMessage(&Msg);

            if (Msg.message == WM_QUIT)
            {
                bIsExit = false;
                break;
            }
        }

        // Handle window being minimized or screen locked
        if (Renderer->IsOccluded()) continue;

        // Renderer Update
        //Renderer->PrepareRender();
        
        FEditorManager::Get().Tick(DeltaTime);

        // World Update
        if (World)
        {
            World->Tick(DeltaTime);
            Renderer->DrawFinalRender();
            World->LateTick(DeltaTime);
        }
        
        // ui Update
        ui.Update();

        // UI입력을 우선으로 처리하므로, 여기에서 업데이트
        APlayerInput::Get().UpdateInput();
        FEditorManager::Get().HandleInput(DeltaTime);
        // APlayerController::Get().ProcessPlayerInput(DeltaTime);
        
        Renderer->PresentFinalRender();
        //Renderer->SwapBuffer();

        // FPS 제한
        if (bShouldLimitFPS)
        {
            LimitFPS(StartTime, Frequency, TargetDeltaTime);
        }
    }

    // End Run
}

void UEngine::LimitFPS(const LARGE_INTEGER& StartTime, const LARGE_INTEGER& Frequency, double TargetDeltaTime) const
{
    double ElapsedTime;
    do
    {
        Sleep(0);

        LARGE_INTEGER CurrentTime;
        QueryPerformanceCounter(&CurrentTime);

        ElapsedTime = static_cast<double>(CurrentTime.QuadPart - StartTime.QuadPart) * 1000.0 / static_cast<double>(Frequency.QuadPart);
    } while (ElapsedTime < TargetDeltaTime);
}


void UEngine::Shutdown()
{
    ShutdownWindow();
}


void UEngine::InitWindow(uint32 InClientWidth, uint32 InClientHeight)
{
	// Register Window Class //
    WNDCLASSW wnd_class{};
    wnd_class.lpfnWndProc = WndProc;
    wnd_class.hInstance = WindowInstance;
    wnd_class.lpszClassName = WindowClassName;
    RegisterClassW(&wnd_class);

    // 파라미터로 전달 받은 Client 크기를 Window 크기로 변경 (윈도우 타이틀 바, 테두리, 메뉴 바 등을 포함한 크기)
    RECT ClientRect = {0, 0, static_cast<int32>(InClientWidth), static_cast<int32>(InClientHeight)};
    AdjustWindowRect(&ClientRect, WS_OVERLAPPEDWINDOW, false);
    int WindowWidth = ClientRect.right - ClientRect.left;
    int WindowHeight = ClientRect.bottom - ClientRect.top;

    // Create Window Handle //
    WindowHandle = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP,
        WindowClassName, WindowTitle,
        WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WindowWidth, WindowHeight,
        nullptr, nullptr, WindowInstance, nullptr
    );

	//@TODO: Implement Fullscreen, Borderless Mode
    if (ScreenMode != EScreenMode::Windowed)
    {
        std::cout << "not implement Fullscreen and Borderless mode." << '\n';
    }

    // Focus Window //
    ShowWindow(WindowHandle, SW_SHOW);
    SetForegroundWindow(WindowHandle);
    SetFocus(WindowHandle);
}

void UEngine::InitRenderer()
{
    Renderer = std::make_unique<URenderer>();
    Renderer->Create(WindowHandle);
    Renderer->CreateShader();
    Renderer->CreateConstantBuffer();

    Renderer->GenerateWorldGridVertices(WorldGridCellPerSide);
}

void UEngine::InitWorld()
{
    World = FObjectFactory::ConstructObject<UWorld>();

	// Add ActorTreeNode to World->ActorTreeNodes //
    World->WorldNode = new ActorTreeNode(*World->GetName(), *World->GetClass()->Name, nullptr, World->GetUUID(), nullptr);
	World->ActorTreeNodes.Add(World->WorldNode);

    if (ACamera* Camera = World->SpawnActor<ACamera>())
    {
        FEditorManager::Get().SetCamera(Camera);

        Camera->OnCameraMoved.Bind(Renderer.get(), &URenderer::UpdateViewMatrix);
        Camera->OnCameraProjectionChanged.Bind(Renderer.get(), &URenderer::UpdateProjectionMatrix);
        
        InitEditorCameraWithEngineConfig(Camera);
    }
    
    World->SpawnActor<APicker>();
    FEditorManager::Get().SetGizmoHandle(World->SpawnActor<AGizmoHandle>());

    World->BeginPlay();
}

void UEngine::InitEditorCameraWithEngineConfig(ACamera* InCamera)
{
    // Default values
    FVector CameraLocation(4.f, -3.f, 3.f);
    FQuat CameraRotation(0.f, 0.f, 0.f, 1.f);
    FTransform CameraTransform(CameraLocation, CameraRotation, FVector::OneVector);

    float CameraSpeed = 5.f;
    float CameraSensitivity = 10.f;
    
    if (EngineConfig->IsSectionExist(EEngineConfigSectionType::ECS_Camera))
    {
        // 카메라 ini 읽어오기
        const float LocX = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosX, 4.f);
        const float LocY = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosY, -3.f);
        const float LocZ = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraPosZ, 3.f);

        const float RotX = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotX);
        const float RotY = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotY);
        const float RotZ = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotZ);
        const float RotW = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraRotW, 1.f);

        CameraLocation = FVector(LocX, LocY, LocZ);
        CameraRotation = FQuat(RotX, RotY, RotZ, RotW);
        CameraTransform = FTransform(CameraLocation, CameraRotation, FVector::OneVector);
    
        CameraSpeed = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraSpeed, 5.f);
        CameraSensitivity = EngineConfig->GetEngineConfigValue<float>(EEngineConfigValueType::EEC_EditorCameraSensitivity, 10.f);
    }
    else
    {
        // config가 없는 경우 원점을 바라보게 하기 위해 여기에서 섹션이 존재하는지 확인하고 로테이션 계산
        CameraTransform.LookAt(FVector(0.f, 0.f, 0.f));
        CameraRotation = CameraTransform.GetRotation(); // LookAt 함수가 FTransform에만 있어서 CameraRotation을 재설정 해야함.
    }
    
    InCamera->SetActorTransform(CameraTransform);
    
    APlayerController::Get().SetCurrentSpeed(CameraSpeed);
    APlayerController::Get().SetMouseSensitivity(CameraSensitivity);

    // 카메라 초기화 후 상수 버퍼 업데이트
    Renderer->UpdateProjectionMatrix(InCamera);

    // Update all camera config
    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosX, CameraLocation.X);
    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosY, CameraLocation.Y);
    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraPosZ, CameraLocation.Z);

    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotX, CameraRotation.X);
    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotY, CameraRotation.Y);
    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotZ, CameraRotation.Z);
    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraRotW, CameraRotation.W);

    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraSpeed, CameraSpeed);
    EngineConfig->UpdateEngineConfig(EEngineConfigValueType::EEC_EditorCameraSensitivity, CameraSensitivity);
}

void UEngine::InitTextureLoader()
{
    // TextureLoader 생성
    TextureLoaderInstance = new TextureLoader(Renderer->GetDevice(), Renderer->GetDeviceContext());

	// Texture Load
    bool bLoaded = true;
    bLoaded |= LoadTexture(TEXT("ASCII"), TEXT("ASCII.png"), 16, 16);
}

void UEngine::ShutdownWindow()
{
    DestroyWindow(WindowHandle);
    WindowHandle = nullptr;

    UnregisterClassW(WindowClassName, WindowInstance);
    WindowInstance = nullptr;

    ui.Shutdown();
    
    EngineConfig->SaveAllConfig();
	delete EngineConfig;
}

void UEngine::UpdateWindowSize(const uint32 InClientWidth, const uint32 InClientHeight)
{
    ClientWidth = InClientWidth;
    ClientHeight = InClientHeight;
    
    if(Renderer)
    {
        Renderer->OnClientSizeUpdated(InClientWidth, InClientHeight);
    }

    if (ui.bIsInitialized)
    {
        ui.OnClientSizeUpdated(InClientWidth, InClientHeight);
    }

    APlayerInput::Get().SetClientSize(ClientWidth, ClientHeight);

	EngineConfig->UpdateEngineConfig<uint32>(EEngineConfigValueType::EEC_ScreenWidth, ClientWidth);
	EngineConfig->UpdateEngineConfig<uint32>(EEngineConfigValueType::EEC_ScreenHeight, ClientHeight);
}

UObject* UEngine::GetObjectByUUID(uint32 InUUID) const
{
    if (const auto Obj = GObjects.Find(InUUID))
    {
        return Obj->get();
    }
    return nullptr;
}

bool UEngine::LoadTexture(const FName& Name, const FString& FileName, int32 Rows, int32 Columns)
{
	if (TextureLoaderInstance)
	{
		return TextureLoaderInstance->LoadTexture(Name, FileName, Rows, Columns);
	}
    return false;
}

 TextureInfo* UEngine::GetTextureInfo(const FName& Name) const
{
    if (TextureLoaderInstance)
    {
		return TextureLoaderInstance->GetTextureInfo(Name);
    }
    return nullptr;
}
