#include "pch.h"
#include "BillboardComponent.h"
#include "Engine/GameFrameWork/Camera.h"
#include "World.h"

REGISTER_CLASS(UBillboard);
UBillboardComponent::UBillboardComponent()
	: Texture(nullptr)
{
	bCanBeRendered = false;
}

void UBillboardComponent::BeginPlay()
{
    Super::BeginPlay();
	GetOwner()->GetWorld()->AddBillboardComponent(this);
}

void UBillboardComponent::Render(class URenderer* Renderer)
{
    if (Renderer == nullptr || Texture == nullptr || !bCanBeRendered)
    {
        return;
    }

    // 텍스처와 샘플러 상태를 셰이더에 설정
    Renderer->GetDeviceContext()->PSSetShaderResources(0, 1, &Texture);

    Renderer->UpdateTextureConstantBuffer(GetWorldTransform().GetMatrix(), RenderCol/TotalCols, RenderRow/TotalRows, TotalCols, TotalRows);
    // 렌더링
    Renderer->RenderBillboard();
}

void UBillboardComponent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void UBillboardComponent::EndPlay(const EEndPlayReason::Type Reason)
{
    GetOwner()->GetWorld()->RemoveBillboardComponent(this);
}

void UBillboardComponent::SetTexture(ID3D11ShaderResourceView* InTexture, float Cols, float Rows)
{
    Texture = InTexture;
    TotalCols = Cols;
    TotalRows = Rows;
}

void UBillboardComponent::SetRenderUV(float Col, float Row)
{
	RenderCol = Col;
	RenderRow = Row;
}


