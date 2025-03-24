#pragma once
#include "Input/PlayerInput.h"

struct FRect
{
    int32 Left;
    int32 Top;
    int32 Right;
    int32 Bottom;
};

class SCompoundWidget
{
public:
    SCompoundWidget() = default;
    virtual ~SCompoundWidget() = default;

    virtual void Init(const FRect& InRect);
    
    virtual void Tick(const float DeltaTime) {}

    virtual bool IsHover(FPoint coord) const { return false;}

    virtual void Paint() {}

    virtual void HandleInput(const float DeltaTime) {}

    // Client 기준으로 Left Top이 (0, 0)
    FRect Rect;
    
protected:
};
