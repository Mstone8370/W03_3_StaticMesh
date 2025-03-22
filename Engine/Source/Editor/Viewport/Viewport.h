#pragma once

struct FPoint;

struct FRect
{
    int32 Left;
    int32 Top;
    int32 Right;
    int32 Bottom;
};

class SWindow
{
public:
    SWindow();
    ~SWindow() = default;
    
    bool IsHover(FPoint coord) const;

protected:
    FRect Rect;
};

class SSplitter : public SWindow
{
public:
    SSplitter();
    ~SSplitter() = default;
    
    SWindow* SideLT; // Left or Top
    SWindow* SideRB; // Right or Bottom
};

class SSplitterH : public SSplitter
{
};

class SSplitterV : public SSplitter
{
};