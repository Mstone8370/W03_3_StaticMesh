﻿#pragma once
#include "Core/Container/String.h"
#include "Core/Container/Map.h"

class UClass;

#define UCLASS(ClassName, ParentClass)                                                    \
public:                                                                                   \
    static UClass* StaticClass()                                                          \
    {                                                                                     \
		static UClass StaticClassInstance(#ClassName, ParentClass::StaticClass());        \
        return &StaticClassInstance;                                                      \
    }                                                                                     \
    virtual UClass* GetClass() const { return StaticClass(); }                            \
private:                                                                                  \
    struct AutoRegister_##ClassName                                                       \
    {                                                                                     \
        AutoRegister_##ClassName()                                                        \
        {                                                                                 \
            UClass::RegisterClass(StaticClass());                                         \
        }                                                                                 \
    };                                                                                    \
inline static AutoRegister_##ClassName AutoRegisterInstance_##ClassName;                  \
using Super = ParentClass;


class UClass
{
public:
	FString Name;
	UClass* SuperClass;

	UClass() = default;

	UClass(const FString& InName, UClass* InParentClass )
		: Name(InName), SuperClass(InParentClass){}

	static TMap<FString, UClass*>& GetRegistry()
	{
		static TMap<FString, UClass*> Registry;
		return Registry;
	}

	static UClass* FindClass(const FString& ClassName)
	{
		auto& Registry = GetRegistry();
		auto Found = Registry.Find(ClassName);
		if (Found == nullptr)
		{
			return nullptr;
		}
		return *Found;
	}

	static void RegisterClass(UClass* Class)
	{
		GetRegistry()[Class->Name] = Class;
	}

	// 부모 클래스인지 확인
	bool IsChildOf(const UClass* ParentClass) const
	{
		const UClass* Class = this;
		while (Class)
		{
			if (Class == ParentClass)
			{
				return true;
			}
			Class = Class->SuperClass;
		}
		return false;
	}

	// UClass 자체의 StaticClass 메서드 정의
	static UClass* StaticClass()
	{
		static UClass RootClass("UClass", nullptr);
		return &RootClass;
	}

	template<typename T>
	bool IsA() const
	{
		return IsChildOf(T::StaticClass());
	}
};

