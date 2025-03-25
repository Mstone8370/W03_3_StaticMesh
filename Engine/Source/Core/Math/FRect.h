#pragma once
#include "Vector.h"

struct FPoint
{
    float X, Y;
    FPoint() : X(0), Y(0) {}
    FPoint(float InX, float InY) : X(InX), Y(InY) {}
};

struct FRect
{
    float X, Y, Width, Height;

    FRect() : X(0), Y(0), Width(0), Height(0) {}
    FRect(float InX, float InY, float InW, float InH) : X(InX), Y(InY), Width(InW), Height(InH) {}

    bool Contains(const FPoint& Point) const
    {
        return Point.X >= X && Point.X <= (X + Width) &&
               Point.Y >= Y && Point.Y <= (Y + Height);
    }

    FVector2D GetMin() const { return FVector2D(X, Y); }
    FVector2D GetMax() const { return FVector2D(X + Width, Y + Height); }
};