#pragma once

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
    ~SCompoundWidget() = default;

    virtual void Paint() {}
    virtual void Tick(const float DeltaTime) {}
    
};