#pragma once
#include "Viewport.h"
#include "SlateCore/SCompoundWidget.h"

class SViewport : public SCompoundWidget
{
public:
    SViewport();
    ~SViewport() = default;

    virtual void Paint() override;

private:
    std::unique_ptr<FViewport> Viewport;
};
