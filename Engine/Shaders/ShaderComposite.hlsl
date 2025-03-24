// ShaderComposite.hlsl

cbuffer CbCompositeInfo : register(b0)
{
    float2 ViewportSize;       // 뷰포트 크기 (in pixels)
    float2 ViewportPosition;   // 뷰포트 좌상단 위치 (in pixels)
};

cbuffer CbScreenInfo : register(b3)
{
    float2 ScreenSize;         // 전체 FrameBuffer 크기 (in pixels)
    float2 Padding;            // 16바이트 정렬용
};

Texture2D CompositeTexture : register(t0);
SamplerState TextureSampler : register(s0);

struct VS_INPUT
{
    float3 Pos : POSITION;   // clip-space 기준: -1 ~ +1
    float2 UV  : TEXCOORD0;  // 0 ~ 1 범위
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV  : TEXCOORD0;
};
/*
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    float2 Scale = float2(2.9f, 2.9f);
    // Viewport의 중앙을 기준으로 확대
    float2 VP_Center = ViewportPosition + ViewportSize * 0.5f;
    float2 HalfSize = ViewportSize * 0.5f * Scale;
    // 픽셀 기준 좌상단, 우하단 좌표
    //float2 ViewportMin = ViewportPosition;
    //float2 ViewportMax = ViewportPosition + ViewportSize * Scale;
    float2 ViewportMin = VP_Center - HalfSize;
    float2 ViewportMax = VP_Center + HalfSize;
    // UV (0~1) 기준으로 Viewport 내 픽셀 위치 보간
    float2 PixelPos = lerp(ViewportMin, ViewportMax, input.UV);

    // 픽셀 위치를 clip-space로 변환
    float2 ClipPos = PixelPos / ScreenSize * 2.0f - 1.0f;

    // Flip Y-axis (DirectX 좌표계 보정)
    ClipPos.y *= -1.0f;

    output.Pos = float4(ClipPos, 0.0f, 1.0f);
    output.UV = input.UV;
    return output;
}
*/
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    // input.Pos 은 [-1, 1], input.UV는 [0, 1]
    output.Pos = float4(input.Pos.xy, 0.0f, 1.0f);
    output.UV  = input.UV;
    return output;
}
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    //return float4(0,0,1,1);
    return CompositeTexture.Sample(TextureSampler, input.UV);
}
