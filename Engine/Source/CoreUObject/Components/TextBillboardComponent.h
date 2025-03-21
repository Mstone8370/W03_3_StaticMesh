#pragma once
#include "BillboardComponent.h"

class UTextBillboardComponent : public UBillboardComponent
{
	UCLASS(UTextBillboard, UBillboardComponent)

public:
	UTextBillboardComponent();
	virtual ~UTextBillboardComponent() = default;
	
	virtual void BeginPlay() override;
	
	virtual void Render(class URenderer* Renderer) override;
	
	virtual void EndPlay(const EEndPlayReason::Type Reason) override;
	
	void SetText(const std::wstring& InString);

private:
	std::wstring TextString;
};



