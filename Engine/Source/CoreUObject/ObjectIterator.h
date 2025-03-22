#pragma once
#include "Engine.h"
#include "CoreUObject/Object.h"

template<typename T>
class TObjectIterator
{
public:
    TObjectIterator()
    {
        auto& Objects = UEngine::Get().GObjects;
        Current = Objects.begin();
        End = Objects.end();
        Advance();
    }

    TObjectIterator& operator++()
    {
        ++Current;
        Advance();
        return *this;
    }
    TObjectIterator operator++(int)
    {
        TObjectIterator temp = *this;
        ++(*this);
        return temp;
    }
    T* operator*() const
    {
        auto& Pair = *Current;
        return static_cast<T*>(Pair.Value.get());
    }

    bool operator!=(const TObjectIterator& Other) const
    {
        return Current != Other.Current;
    }
    explicit operator bool() const 
    {
        return Current != End;
    }

private:
    TMap<uint32, std::shared_ptr<UObject>>::Iterator Current;
    TMap<uint32, std::shared_ptr<UObject>>::Iterator End;

    bool Advance()
    {
        while (Current != End)
        {
            if (Current->Value->IsA<T>())
            {
                return true;
            }
            ++Current;
        }
        return false;
    }
};
