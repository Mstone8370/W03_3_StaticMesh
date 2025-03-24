#pragma once
#include "SCompoundWidget.h"
#include "Slate/SViewport.h"

struct FPoint;

// 뷰포트 윈도우의 기본 클래스
class SWindow : public SCompoundWidget
{
public:
    SWindow();
    virtual ~SWindow() override;

    virtual void Init(const FRect& InRect) override;

    virtual void Tick(const float DeltaTime) override;
    
    virtual bool IsHover(FPoint coord) const override;

    virtual void Paint() override;

    virtual void HandleInput(const float DeltaTime) override;

private:
    std::unique_ptr<SViewport> Viewport;
    
};

// 윈도우를 반으로 나누는 기능이 있는 윈도우로, 반으로 나뉘며 둘로 나눠진 윈도우를 참조 중
class SSplitter : public SWindow
{
public:
    SSplitter();
    virtual ~SSplitter() override = default;

    virtual void Paint() override;

    virtual void HandleInput(const float DeltaTime) override;
    
private:
    // TODO: 아래의 두 포인터를 어떻게 관리해야할지 생각하기
    SWindow* SideLT; // Left or Top
    SWindow* SideRB; // Right or Bottom
};

class SSplitterH : public SSplitter
{
};

class SSplitterV : public SSplitter
{
};