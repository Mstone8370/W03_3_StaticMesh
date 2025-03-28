#include "pch.h"
#include "World.h"

#include "WorldGrid.h"
#include "Core/Input/PlayerInput.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameFrameWork/Camera.h"
#include "Core/Container/Map.h"
#include "Utils/JsonSavehelper.h"

#include "Engine/GameFrameWork/Cone.h"
#include "Engine/GameFrameWork/Cube.h"
#include "Engine/GameFrameWork/Cylinder.h"
#include "Engine/GameFrameWork/Sphere.h"
#include "Engine/GameFrameWork/AStaticMesh.h"
#include "Input/PlayerController.h"

#include "Components/BillboardComponent.h"
#include "Components/TextBillboardComponent.h"
#include "GameFrameWork/Picker.h"
#include "CoreUObject/ObjectIterator.h"
#include "Editor/Viewport/FViewport.h"

void UWorld::BeginPlay()
{
    for (const auto& Actor : Actors)
    {
        Actor->BeginPlay();
    }
}

void UWorld::Tick(float DeltaTime)
{
    for (const auto& Actor : ActorsToSpawn)
    {
        Actor->BeginPlay();
    }
    ActorsToSpawn.Empty();

    const auto CopyActors = Actors;
    for (const auto& Actor : CopyActors)
    {
        if (Actor->CanEverTick())
        {
            Actor->Tick(DeltaTime);
        }
    }
}

void UWorld::LateTick(float DeltaTime)
{
    const auto CopyActors = Actors;
    for (const auto& Actor : CopyActors)
    {
        if (Actor->CanEverTick())
        {
            Actor->LateTick(DeltaTime);
        }
    }

    for (const auto& PendingActor : PendingDestroyActors)
    {
        // Remove from Engine
        UEngine::Get().GObjects.Remove(PendingActor->GetUUID());
    }
    PendingDestroyActors.Empty();
}

void UWorld::Render(float DeltaTime)
{
    //UWorld의 렌더 함수 부분을 Renderer의 RenderViewports로 이전
    URenderer* Renderer = UEngine::Get().GetRenderer();

    if (Renderer == nullptr)
    {
        return;
    }

    //ACamera* Camera = FEditorManager::Get().GetCamera();
    //Renderer->RenderViewport(Camera, this);
    /**
     * Axis는 Grid에 가려지면 안되므로 Grid 먼저 렌더.
     * Axis는 아래의 RenderMainTexture 함수에서 렌더됨.
     */
    /*
    RenderWorldGrid(*Renderer);
        
    //if (!APlayerController::Get().IsUiInput() && APlayerInput::Get().IsMousePressed(false))
    //    RenderPickingTexture(*Renderer);
    RenderMainTexture(*Renderer);
	RenderBillboard(*Renderer);
    RenderText(*Renderer);
    RenderMesh(*Renderer);
    
	RenderBoundingBoxes(*Renderer);
    RenderDebugLines(*Renderer, DeltaTime);

    // DisplayPickingTexture(*Renderer);*/
}

void UWorld::RenderPickingTexture(URenderer& Renderer)
{
    Renderer.PreparePicking();
    Renderer.PreparePickingShader();

    for (auto& RenderComponent : RenderComponents)
    {
        uint32 UUID = RenderComponent->GetUUID();
        RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
        RenderComponent->Render(&Renderer);
    }

    Renderer.RenderGizmo(FEditorManager::Get().GetGizmoHandle());
}

void UWorld::RenderPickingTextureForViewport(URenderer& Renderer, FViewport& View)
{
    if (!View.PickingRTV || !View.PickingDSV)
        return;

    ID3D11DeviceContext* Context = Renderer.GetDeviceContext();

    // 1. 렌더 타겟 바인딩
    Context->OMSetRenderTargets(1, &View.PickingRTV, View.PickingDSV);
    Context->ClearRenderTargetView(View.PickingRTV, Renderer.PickingClearColor);
    Context->ClearDepthStencilView(View.PickingDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // 2. 뷰포트 설정
    Context->RSSetViewports(1, &View.ViewportDesc);

    // 3. View / Projection 행렬 설정
    if (View.ViewCamera)
    {
        Renderer.UpdateViewMatrix(View.ViewCamera->GetActorTransform());
        Renderer.UpdateProjectionMatrixAspect(View.ViewCamera, View.ViewportDesc.Width, View.ViewportDesc.Height);
    }

    // 4. Pickable 오브젝트 렌더링
    Renderer.PreparePicking();
    Renderer.PreparePickingShader();

    for (UPrimitiveComponent* Component : RenderComponents)
    {
        if (!Component) continue;

        uint32 UUID = Component->GetUUID();
        Component->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
        Component->Render(&Renderer);
    }

    // 5. 기즈모 렌더링 (깊이 무시)
    Renderer.ClearCurrentDepthSencilView();
    Renderer.RenderGizmoPicking(FEditorManager::Get().GetGizmoHandle());
}


void UWorld::RenderMainTexture(URenderer& Renderer)
{
    Renderer.PrepareMain();
    Renderer.PrepareMainShader();

    bool bRenderPrimitives = UEngine::Get().GetShowPrimitives();
    for (auto& RenderComponent : RenderComponents)
    {
        if (RenderComponent->IsA<UMeshComponent>())
        {
            continue;
        }
        if (!bRenderPrimitives && !RenderComponent->GetOwner()->IsGizmoActor())
        {
            continue;
        }
        RenderComponent->Render(&Renderer);
    }
}

void UWorld::RenderMesh(URenderer& Renderer)
{
    Renderer.PrepareMesh();
    Renderer.PrepareMeshShader();

    bool bRenderPrimitives = UEngine::Get().GetShowPrimitives();
    for (auto& RenderComponent : RenderComponents)
    {
        if (!RenderComponent->IsA<UMeshComponent>())
        {
            continue;
        }
        if (!bRenderPrimitives && !RenderComponent->GetOwner()->IsGizmoActor())
        {
            continue;
        }
        RenderComponent->Render(&Renderer);
    }
}

void UWorld::RenderBoundingBox(URenderer& Renderer)
{
    if (USceneComponent* SelectedSceneComp = FEditorManager::Get().GetSelectedComponent())
    {
        if (FBox* Box = SelectedSceneComp->GetBoundingBox().get())
        {
            Renderer.PrepareMain();
            Renderer.PrepareMainShader();
            
            Renderer.RenderBox(*Box);
        }
    }
}

void UWorld::RenderWorldGrid(URenderer& Renderer)
{
    Renderer.RenderWorldGrid();
}

void UWorld::RenderDebugLines(URenderer& Renderer, float DeltaTime)
{
    Renderer.RenderDebugLines(DeltaTime);
}

void UWorld::RenderBillboard(URenderer& Renderer)
{
    // 텍스처와 샘플러 상태를 셰이더에 설정
    Renderer.PrepareBillboard();

    for (UBillboardComponent* Billboard : BillboardComponents)
    {
        if (Billboard)
        {
            Billboard->Render(&Renderer);
        }
    }
}

void UWorld::RenderText(URenderer& Renderer)
{
    // 텍스처와 샘플러 상태를 셰이더에 설정
    Renderer.PrepareTextBillboard();

    for (UTextBillboardComponent* TextBillboard : TextBillboardComponents)
    {
        if (TextBillboard)
        {
            TextBillboard->Render(&Renderer);
        }
    }
}

void UWorld::DisplayPickingTexture(URenderer& Renderer)
{
    Renderer.RenderPickingTexture();
}

void UWorld::ClearWorld()
{
    TArray CopyActors = Actors;
    for (AActor* Actor : CopyActors)
    {
        if (!Actor->IsGizmoActor())
        {
            DestroyActor(Actor);
        }
    }

    UE_LOG("Clear World");
}


bool UWorld::DestroyActor(AActor* InActor)
{
    //@TODO: 나중에 Destroy가 실패할 일이 있다면 return false; 하기
    assert(InActor);

    if (PendingDestroyActors.Find(InActor) != -1)
    {
        return true;
    }

    // 삭제될 때 Destroyed 호출
    InActor->Destroyed();

    // 삭제하고자 하는 Actor를 가지고 있는 ActorTreeNode를 찾아서 삭제
    for (ActorTreeNode* Node : ActorTreeNodes)
    {
        if (Node->GetActor() == InActor)
        {
            Node->GetParent()->RemoveChild(Node);
            for (ActorTreeNode* Child : Node->GetChildren())
            {
                Child->SetParent(Node->GetParent());
            }
            ActorTreeNodes.Remove(Node);
            break;
        }
    }

    // World에서 제거
    Actors.Remove(InActor);

    // 제거 대기열에 추가
    PendingDestroyActors.Add(InActor);

    return true;
}

void UWorld::SaveWorld()
{
    const UWorldInfo& WorldInfo = GetWorldInfo();
    JsonSaveHelper::SaveScene(WorldInfo);
}

void UWorld::LoadWorld(const char* InSceneName)
{
    if (InSceneName == nullptr || strcmp(InSceneName, "") == 0)
    {
        return;
    }

    UWorldInfo* WorldInfo = JsonSaveHelper::LoadScene(InSceneName);
    if (WorldInfo == nullptr) return;

    ClearWorld();

    Version = WorldInfo->Version;
    this->SceneName = WorldInfo->SceneName;
    uint32 ActorCount = WorldInfo->ActorCount;

    // Check Type
    for (uint32 i = 0; i < ActorCount; i++)
    {
        UObjectInfo* ObjectInfo = WorldInfo->ObjctInfos[i];
        FTransform Transform = FTransform(ObjectInfo->Location, FQuat(), ObjectInfo->Scale);
        Transform.Rotate(ObjectInfo->Rotation);

        AActor* Actor = nullptr;

        if (ObjectInfo->ObjectType == "Actor")
        {
            Actor = SpawnActor<AActor>();
        }
        else if (ObjectInfo->ObjectType == "Sphere")
        {
            Actor = SpawnActor<ASphere>();
        }
        else if (ObjectInfo->ObjectType == "Cube")
        {
            Actor = SpawnActor<ACube>();
        }
        else if (ObjectInfo->ObjectType == "Arrow")
        {
            Actor = SpawnActor<AArrow>();
        }
        else if (ObjectInfo->ObjectType == "Cylinder")
        {
            Actor = SpawnActor<ACylinder>();
        }
        else if (ObjectInfo->ObjectType == "Cone")
        {
            Actor = SpawnActor<ACone>();
        }
        else if (ObjectInfo->ObjectType == "AStaticMesh")
        {
            Actor = SpawnActor<AStaticMesh>();
            if (Actor)
            {
                AStaticMesh* MeshActor = static_cast<AStaticMesh*>(Actor);
                MeshActor->SetAssetName(ObjectInfo->AssetName);
                MeshActor->InitStaticMeshBoundingBox();
            }
        }

        if (Actor)
        {
            Actor->SetActorTransform(Transform);
        }
    }
}

UWorldInfo UWorld::GetWorldInfo() const
{
    UWorldInfo WorldInfo;
    WorldInfo.ActorCount = Actors.Num();
    WorldInfo.ObjctInfos = new UObjectInfo*[WorldInfo.ActorCount];
    WorldInfo.SceneName = std::string(SceneName.c_char());
    WorldInfo.Version = 1;
    uint32 i = 0;
    for (auto& actor : Actors)
    {
        if (actor->IsGizmoActor())
        {
            WorldInfo.ActorCount--;
            continue;
        }
        WorldInfo.ObjctInfos[i] = new UObjectInfo();
        const FTransform& Transform = actor->GetActorTransform();
        WorldInfo.ObjctInfos[i]->Location = Transform.GetLocation();
        WorldInfo.ObjctInfos[i]->Rotation = Transform.GetRotation().GetEuler();
        WorldInfo.ObjctInfos[i]->Scale = Transform.GetScale();
        WorldInfo.ObjctInfos[i]->ObjectType = actor->GetTypeName();
        if (actor->GetTypeName() == "AStaticMesh") {
            AStaticMesh* StaticMeshActor = static_cast<AStaticMesh*>(actor);
            WorldInfo.ObjctInfos[i]->AssetName = StaticMeshActor->GetAssetName().c_char();
        }

        WorldInfo.ObjctInfos[i]->UUID = actor->GetUUID();
        i++;
    }
    return WorldInfo;
}

bool UWorld::LineTrace(const FRay& Ray, USceneComponent** FirstHitComponent) const
{
    TArray<TPair<USceneComponent*, float>> Hits;
    for (TObjectIterator<UPrimitiveComponent> It; It; ++It)
    {
        if (UPrimitiveComponent* PrimitiveComp = *It)
        {
            if (!PrimitiveComp->bCanBeRendered)
            {
                continue;
            }
            
            if (FBox* Box = PrimitiveComp->GetBoundingBox().get())
            {
                float Distance = 0.f;
                if (Box && Box->IsValidBox() && Box->IntersectRay(Ray, Distance))
                {
                    Hits.Add({Box->GetOwner(), Distance});
                }
            }
        }
    }
    if (Hits.Num() == 0)
    {
        if (bDebugRaycast)
        {
            FVector Start = Ray.Origin;
            FVector End = Ray.Origin + Ray.Direction * Ray.Length;
            DrawDebugLine(Start, End, FVector(1.f, 0.f, 0.f), 10.f);
        }
        return false;
    }

    // Find min dist
    float MinDistance = FLT_MAX;
    for (const auto& [SceneComp, Dist] : Hits)
    {
        if (!SceneComp)
        {
            
        }
        if (Dist < MinDistance&&SceneComp)
        {
            MinDistance = Dist;
            *FirstHitComponent = SceneComp;
        }
    }
    if (bDebugRaycast)
    {
        FVector Start = Ray.Origin;
        FVector Mid = Ray.Origin + Ray.Direction * MinDistance;
        FVector End = Ray.Origin + Ray.Direction * Ray.Length;

        DrawDebugLine(Start, Mid, FVector(1.f, 0.f, 0.f), 5.f);  // Hit 구간: 빨강
        DrawDebugLine(Mid, End, FVector(0.f, 1.f, 0.f), 5.f);    // 잔여 구간: 초록

        //UE_LOG("Ray Hit: %s | Distance: %.2f", *(*FirstHitComponent)->GetName(), MinDistance);
    }

    return true;
}

void UWorld::DrawDebugLine(FVector Start, FVector End, FVector Color, float Time) const
{
    if (URenderer* Renderer = UEngine::Get().GetRenderer())
    {
        Renderer->AddDebugLine(Start, End, Color, Time);
    }
}
