#pragma once
#include "Viewport.h"
#include "SlateCore/SCompoundWidget.h"

class SViewport : public SCompoundWidget
{
public:
    SViewport();
    virtual ~SViewport() override;

    virtual void Paint() override;

    virtual void HandleInput(const float DeltaTime) override;

private:
    std::unique_ptr<FViewport> Viewport;
};
