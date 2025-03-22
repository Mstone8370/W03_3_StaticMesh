// Shaders/FullscreenQuad.hlsl

cbuffer TextureConstants : register(b5)
{
    float4x4 WorldViewProj;
    float U;
    float V;
    float TotalCols;
    float TotalRows;
    int bIsText;
};

Texture2D FullscreenTexture : register(t0);
SamplerState Sampler : register(s0);

struct VSInput
{
    float3 Position : POSITION;
    float2 UV       : TEXCOORD0;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD0;
};

VSOutput mainVS(VSInput input)
{
    VSOutput output;
    output.Position = float4(input.Position, 1.0f); // Clip space에서 위치 고정
    output.UV = input.UV;
    return output;
}

float4 mainPS(VSOutput input) : SV_Target
{
    return FullscreenTexture.Sample(Sampler, input.UV);
}
