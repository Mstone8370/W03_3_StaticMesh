#pragma once

#include "Core/Container/Set.h"
#include "Core/Math/Transform.h"
#include "CoreUObject/Object.h"
#include "CoreUObject/ObjectFactory.h"
#include "CoreUObject/Components/ActorComponent.h"
#include "CoreUObject/Components/SceneComponent.h"
#include "CoreUObject/Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"

class UWorld;

class AActor : public UObject
{
	UCLASS(AActor, UObject);
	friend class FEditorManager;
public:
	AActor();
	virtual ~AActor() override = default;

public:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void LateTick (float DeltaTime); // 렌더 후 호출

	// Called when this actor is explicitly destroyed.
	virtual void Destroyed();
	// TODO: EndPlay는 추후 삭제
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	TSet<UActorComponent*>& GetComponents() { return Components; }

	UWorld* GetWorld() const { return World; }
	void SetWorld(UWorld* InWorld) { World = InWorld; }

	bool IsGizmoActor() const { return bIsGizmo; }

	void SetBoundingBoxRenderable(bool bRenderable);

	virtual FVector GetActorForwardVector() const { return GetActorTransform().GetForward(); }
	virtual FVector GetActorRightVector() const { return GetActorTransform().GetRight(); }
	virtual FVector GetActorUpVector() const { return GetActorTransform().GetUp(); }

	virtual void SetActorTransform(const FTransform& InTransform);
	
protected:
	virtual void InitUUIDBillboard();
private:
	virtual void Pick();
	virtual void UnPick();

	bool bIsPicked = false;

public:
	bool IsPicked() const { return bIsPicked; }

public:
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	T* AddComponent();

	// delete
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	void RemoveComponent(T* Object)
	{
		Components.Remove(Object);
	}

	template<typename T>
		requires std::derived_from<T, UActorComponent>
	T* FindComponent() const;

	FTransform GetActorTransform() const;
	bool CanEverTick() const { return bCanEverTick; }
	virtual const char* GetTypeName();

	// Destroy this actor.
	bool Destroy();

public:
	USceneComponent* GetRootComponent() const { return RootComponent; }
	void SetRootComponent(USceneComponent* InRootComponent) { RootComponent = InRootComponent; }

public:
	void SetColor(FVector4 InColor);
	void SetUseVertexColor(bool bUseVertexColor);

protected:
	bool bCanEverTick = true;
	bool bUseBoundingBox = true;
	bool bRenderBoundingBox = false; 

	USceneComponent* RootComponent = nullptr;
	bool bIsGizmo = false;

private:
	UWorld* World = nullptr;
	TSet<UActorComponent*> Components;

	class UTextBillboardComponent* UUIDBillboard = nullptr;
	bool bIsUUIDBillboard = false;

	TSet<FString> ComponentNames;
};


template <typename T> requires std::derived_from<T, UActorComponent>
T* AActor::AddComponent()
{
	T* ObjectInstance = FObjectFactory::ConstructObject<T>();
	Components.Add(ObjectInstance);
	ObjectInstance->SetOwner(this);

	FString ObjectName = ObjectInstance->GetClass()->Name;
	if (ComponentNames.Contains(ObjectName))
	{
		uint32 Count = 0;
		ObjectName += "_";
		while (Count < UINT_MAX)
		{
			FString NumToStr = FString(std::to_string(Count));
			FString TempName = ObjectName;
			TempName += NumToStr;
			if (!ComponentNames.Contains(TempName))
			{
				ObjectInstance->SetName(TempName);
				ComponentNames.Add(TempName);
				break;
			}
			++Count;
		}

		if (Count == UINT_MAX)
		{
			// TODO: 어떤 동작을 해야할지 고민해봐야 함.
		}
	}
	else
	{
		ObjectInstance->SetName(ObjectName);
		ComponentNames.Add(ObjectName);
	}

	UE_LOG("Component Added: %s", *ObjectName);

	return ObjectInstance;
}

template<typename T> requires std::derived_from<T, UActorComponent>
T* AActor::FindComponent() const
{
	for (UActorComponent* Component : Components)
	{
		if (T* FondComponent = dynamic_cast<T*>(Component))
		{
			return FondComponent;
		}
	}
	return nullptr;
}
