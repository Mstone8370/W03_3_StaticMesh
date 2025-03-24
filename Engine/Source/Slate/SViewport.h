#pragma once
#include "Viewport.h"
#include "SlateCore/SCompoundWidget.h"

/**
 * FViewport가 담기는 위젯
 */
class SViewport : public SCompoundWidget
{
public:
    SViewport();
    virtual ~SViewport() override;

    virtual void Init(const FRect& InRect) override;

    virtual void Paint() override;

    virtual void HandleInput(const float DeltaTime) override;
    
    virtual bool IsHover(FPoint coord) const override;

    std::shared_ptr<FViewport> Viewport;
private:
};
