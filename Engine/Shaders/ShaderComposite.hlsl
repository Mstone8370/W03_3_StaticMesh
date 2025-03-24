// ShaderComposite.hlsl

cbuffer CbCompositeInfo : register(b6)
{
    float2 ViewportSize;       // 뷰포트 크기 (in pixels)
    float2 ViewportPosition;   // 뷰포트 좌상단 위치 (in pixels)
};

cbuffer CbScreenInfo : register(b7)
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
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    float2 UV = input.UV;
    // 1. UV → 픽셀 위치 (Viewport 내부 기준)
    float2 PixelPos = UV * ViewportSize;
    // 2. Viewport 내부 위치를 전체 화면 기준으로 이동
    PixelPos += ViewportPosition;
    // 3. 픽셀 위치 → Clip Space 변환
    float2 ClipPos = PixelPos / ScreenSize * 2.0f - 1.0f;
    // 4. DirectX 좌표계 보정 (Y축 뒤집기)
    ClipPos.y *= -1.0f;

    output.Pos = float4(ClipPos, 0.0f, 1.0f);
    output.UV = UV; // 주의: 절대 보정하지 말 것
    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return CompositeTexture.Sample(TextureSampler, input.UV);
}

/*
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    //return float4(0,0,1,1);
    return CompositeTexture.Sample(TextureSampler, input.UV);
}
*/