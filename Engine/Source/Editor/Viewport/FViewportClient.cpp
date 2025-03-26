#include "pch.h"
#include "FViewportClient.h"

#include "Engine.h"
#include "FViewport.h"
#include "World.h"
#include "GameFrameWork/Camera.h"
#include "Input/PlayerInput.h"
#include "Rendering/RenderContext.h"

void FViewportClient::Draw(FViewport* Viewport, const FRenderContext& Context)
{
    if (!Viewport || !Context.Renderer || !Context.World)
        return;

    // 기본 월드 렌더링 루틴
    Context.World->RenderWorldGrid(*Context.Renderer);
    Context.World->RenderMainTexture(*Context.Renderer);
    Context.World->RenderBillboard(*Context.Renderer);
    Context.World->RenderText(*Context.Renderer);
    Context.World->RenderMesh(*Context.Renderer);
    Context.World->RenderBoundingBoxes(*Context.Renderer);
    Context.World->RenderDebugLines(*Context.Renderer, Context.DeltaTime);

    // Prepare new layer for Axis
    Context.Renderer->ClearCurrentDepthSencilView();
    Context.Renderer->RenderAxis();
    
    // Prepare new layer for Gizmo
    Context.Renderer->ClearCurrentDepthSencilView();
    // TODO: render gizmo here
    Context.Renderer->RenderGizmo(FEditorManager::Get().GetGizmoHandle());
}

void FViewportClient::ProcessInput(FViewport* Viewport, float DeltaTime)
{
    if (!Viewport || !Viewport->GetCamera())
        return;

    ACamera* Camera = Viewport->GetCamera();

    // 기본 WASDQE 입력 처리
    FVector MoveDirection = FVector::Zero();

    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::W))
        MoveDirection += Camera->GetActorForwardVector();
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::S))
        MoveDirection -= Camera->GetActorForwardVector();
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::A))
        MoveDirection -= Camera->GetActorRightVector();
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::D))
        MoveDirection += Camera->GetActorRightVector();
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::Q))
        MoveDirection -= FVector(0, 0, 1);
    if (APlayerInput::Get().IsKeyDown(DirectX::Keyboard::Keys::E))
        MoveDirection += FVector(0, 0, 1);

    if (MoveDirection.Length() > SMALL_NUMBER)
    {
        MoveDirection = MoveDirection.GetSafeNormal();
        FTransform Transform = Camera->GetActorTransform();
        Transform.Translate(MoveDirection * DeltaTime * 5.0f); // 속도는 하드코딩 예시
        Camera->SetActorTransform(Transform);
    }

    // 마우스 회전 (우클릭 중일 때만)
    if (APlayerInput::Get().IsMouseDown(true))
    {
        int32 DeltaX = 0, DeltaY = 0;
        APlayerInput::Get().GetMouseDelta(DeltaX, DeltaY);

        FTransform Transform = Camera->GetActorTransform();
        FVector Euler = Transform.GetRotation().GetEuler();
        Euler.Z += DeltaX * 0.2f;
        Euler.Y += DeltaY * 0.2f;
        Euler.Y = FMath::Clamp(Euler.Y, -89.0f, 89.0f);
        Transform.SetRotation(Euler);
        Camera->SetActorTransform(Transform);

        APlayerInput::Get().FixMouseCursor();
    }

    if (APlayerInput::Get().IsMousePressed(true))
    {
        APlayerInput::Get().CacheCursorPosition();
        ShowCursor(false);
    }
    if (APlayerInput::Get().IsMouseReleased(true))
    {
        ShowCursor(true);
    }
}
