#include "pch.h"
#include "Viewport.h"

#include "Engine.h"
#include "World.h"
#include "Input/PlayerInput.h"

FViewportClient::FViewportClient()
    : EditorCamera(nullptr)
    , ViewMatrix(FMatrix::Identity)
    , ProjectionMatrix(FMatrix::Identity)
{
    EditorCamera = std::make_unique<FEditorCamera>();
    EditorCamera->Transform = FTransform(FVector(-4.f, 0.f, 3.f), FVector::ZeroVector, FVector::OneVector);;
    EditorCamera->FOV = 100.f;
    EditorCamera->NearClip = 0.1f;
    EditorCamera->FarClip = 100.0f;
    EditorCamera->Speed = 10.f;
    EditorCamera->Sensitivity = 0.2f;
    EditorCamera->MaxPitch = 89.9f;
}

void FViewportClient::Init(int32 InWidth, int32 InHeight)
{
    ViewMatrix = EditorCamera->Transform.GetViewMatrix();
    
    ProjectionMatrix = FMatrix::PerspectiveFovLH(
        FMath::DegreesToRadians(EditorCamera->FOV),
        static_cast<float>(InWidth) / static_cast<float>(InHeight),
        EditorCamera->NearClip,
        EditorCamera->FarClip
    );
}

void FViewportClient::Draw(const std::weak_ptr<FViewport>& InViewport)
{
    // TODO: 렌더러에 접근해서 실제 랜더 진행
    URenderer* Renderer = UEngine::Get().GetRenderer();
    std::shared_ptr<FViewport> View = InViewport.lock();
    if (!Renderer || !View)
    {
        return;
    }

    const bool bIsOrthographic = ProjectionMode == ECameraProjectionMode::ECP_Orthographic;

    Viewport = InViewport;

    // 뷰포트 설정 후 clear
    Renderer->SetViewport(View.get());
    Renderer->ClearViewport(View.get());

    // 카메라 matrix 설정
    if (bIsOrthographic)
    {
        // FEditorManager의 Orthogonal Point를 받아와서 전달.
        FVector Location = FEditorManager::Get().GetOrthogonalPoint();
        FVector Rotation = FVector::ZeroVector;
        switch (OrthogonalDirection)
        {
        case EOrthogonalDirection::EOD_Top:
            Rotation = FVector(0.f, 90.f, -90.f);
            break;
        case EOrthogonalDirection::EOD_Bottom:
            Rotation = FVector(0.f, -90.f, 90.f);
            break;
        case EOrthogonalDirection::EOD_Left:
            Rotation = FVector(0.f, 0.f, 90.f);
            break;
        case EOrthogonalDirection::EOD_Right:
            Rotation = FVector(0.f, 0.f, -90.f);
            break;
        case EOrthogonalDirection::EOD_Front:
            Rotation = FVector(0.f, 0.f, 180.f);
            break;
        case EOrthogonalDirection::EOD_Back:
            Rotation = FVector(0.f, 0.f, 0.f);
            break;
        }
        FTransform ViewTransform(Location, Rotation, FVector::OneVector);
        ViewMatrix = ViewTransform.GetViewMatrix();
    }
    Renderer->SetViewMatrix(ViewMatrix);
    Renderer->SetProjectionMatrix(ProjectionMatrix, EditorCamera->NearClip, EditorCamera->FarClip);

    // Render World grid
    Renderer->RenderWorldGrid();
    
    // Render Objects
    if (bIsOrthographic)
    {
        Renderer->SetRenderMode(EViewModeIndex::ERS_Wireframe);
    }
    else
    {
        Renderer->SetRenderMode(EViewModeIndex::ERS_Solid);
    }
    UEngine::Get().GetWorld()->RenderMainTexture(*Renderer);

    UEngine::Get().GetWorld()->RenderBillboard(*Renderer);
    UEngine::Get().GetWorld()->RenderText(*Renderer);
    UEngine::Get().GetWorld()->RenderMesh(*Renderer);
    
    // UEngine::Get().GetWorld()->RenderBoundingBoxes(*Renderer);
    Renderer->RenderDebugLines();
    // Render Bounding Box

    // Render Batch lines (Debug line)

    // Render Gizmo

    // Render Axis
}

void FViewportClient::OnResize(int32 InWidth, int32 InHeight)
{
    if (ProjectionMode == ECameraProjectionMode::ECP_Perspective)
    {
        ProjectionMatrix = FMatrix::PerspectiveFovLH(
            EditorCamera->FOV,
            static_cast<float>(InWidth) / static_cast<float>(InHeight),
            EditorCamera->NearClip,
            EditorCamera->FarClip
        );
    }
    else
    {
        ProjectionMatrix = FMatrix::OrthoLH(
            InWidth,
            InHeight,
            EditorCamera->NearClip,
            EditorCamera->FarClip
        );
    }
}

void FViewportClient::HandleInput(const float DeltaTime)
{
    // TODO: 여기에서 카메라를 직접 조작해야함
    if (!APlayerInput::Get().IsMouseDown(true))
    {
        if (APlayerInput::Get().IsMouseReleased(true))
        {
            /**
             * ShowCursor 함수는 참조 카운트를 하므로, 정확한 횟수만큼 Show 및 Hide 하지 않으면
             * 의도대로 작동하지 않는 문제가 발생함으로 매우 주의해야 함.
             */
            //ShowCursor(true);
        }
        return;
    }
    
    FVector NewVelocity(0, 0, 0);

    FTransform CameraTransform = EditorCamera->Transform;

    // Look
    FPoint Delta = APlayerInput::Get().GetMouseDelta();
    
    FVector NewRotation = CameraTransform.GetRotation().GetEuler();
    NewRotation.Y += EditorCamera->Sensitivity * static_cast<float>(Delta.Y); // Pitch
    NewRotation.Z += EditorCamera->Sensitivity * static_cast<float>(Delta.X); // Yaw

    NewRotation.Y = FMath::Clamp(NewRotation.Y, -EditorCamera->MaxPitch, EditorCamera->MaxPitch);
    CameraTransform.SetRotation(NewRotation);

    if (APlayerInput::Get().IsMousePressed(true))
    {
        // Press 이벤트 발생시 커서 위치를 캐싱하여 해당 위치로 커서를 고정시킴.
        APlayerInput::Get().CacheCursorPosition();
        //ShowCursor(false);
    }
    //APlayerInput::Get().FixMouseCursor();
    
    // Move
    int32 MouseWheel = APlayerInput::Get().GetMouseWheelDelta();
    EditorCamera->Speed += static_cast<float>(MouseWheel) * 0.005f;
    EditorCamera->Speed = FMath::Clamp(EditorCamera->Speed, 1.f, 20.f);

    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::A))
    {
        NewVelocity -= EditorCamera->Transform.GetRightVector();
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::D))
    {
        NewVelocity += EditorCamera->Transform.GetRightVector();
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::W))
    {
        NewVelocity += EditorCamera->Transform.GetForwardVector();
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::S))
        {
        NewVelocity -= EditorCamera->Transform.GetForwardVector();
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::Q))
    {
        NewVelocity -= FVector::UpVector;
    }
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::E))
    {
        NewVelocity += FVector::UpVector;
    }
    if (NewVelocity.Length() > 0.001f)
    {
        NewVelocity = NewVelocity.GetSafeNormal();
    }

    CameraTransform.Translate(NewVelocity * DeltaTime * EditorCamera->Speed);
    EditorCamera->Transform = CameraTransform;
    
    // TODO: update engine config
    
    ViewMatrix = EditorCamera->Transform.GetViewMatrix();

    // UE_LOG("X: %f, Y: %f, Z: %f\n", NewRotation.X, NewRotation.Y, NewRotation.Z);
}

void FViewportClient::SetProjectionMode(ECameraProjectionMode InProjectionMode)
{
    ProjectionMode = InProjectionMode;
}

void FViewportClient::SetOrthogonalDirection(EOrthogonalDirection InOrthogonalDirection)
{
    OrthogonalDirection = InOrthogonalDirection;
}

FViewport::FViewport()
    : ViewportClient(std::make_unique<FViewportClient>())
{}

void FViewport::Init(int32 InTopLeftX, int32 InTopLeftY, int32 InWidth, int32 InHeight)
{
    TopLeftX = InTopLeftX;
    TopLeftY = InTopLeftY;
    Width = InWidth;
    Height = InHeight;

    if (URenderer* Renderer = UEngine::Get().GetRenderer())
    {
        Renderer->InitViewport(this);
    }

    if (ViewportClient)
    {
        ViewportClient->Init(InWidth, InHeight);
    }
}

void FViewport::Draw()
{
    if (ViewportClient)
    {
        ViewportClient->Draw(shared_from_this());
    }
}

void FViewport::OnResize(int32 InWidth, int32 InHeight)
{
    if (ViewportClient)
    {
        ViewportClient->OnResize(InWidth, InHeight);
    }
}

void FViewport::HandleInput(const float DeltaTime)
{
    if (ViewportClient)
    {
        ViewportClient->HandleInput(DeltaTime);
    }
}

ECameraProjectionMode FViewport::GetProjectionMode() const
{
    if (ViewportClient)
    {
        return ViewportClient->GetProjectionMode();
    }
    return ECameraProjectionMode::ECP_Max;
}

void FViewport::SetProjectionMode(ECameraProjectionMode InProjectionMode)
{
    if (ViewportClient)
    {
        ViewportClient->SetProjectionMode(InProjectionMode);
    }
}

EOrthogonalDirection FViewport::GetOrthogonalDirection() const
{
    if (ViewportClient)
    {
        return ViewportClient->GetOrthogonalDirection();
    }
    return EOrthogonalDirection::EOD_Max;
}

void FViewport::SetOrthogonalDirection(EOrthogonalDirection InOrthogonalDirection)
{
    if (ViewportClient)
    {
        ViewportClient->SetOrthogonalDirection(InOrthogonalDirection);
    }
}
