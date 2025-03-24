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

    virtual void Paint() override;

    virtual void HandleInput(const float DeltaTime) override;

private:
    std::shared_ptr<FViewport> Viewport;
};
