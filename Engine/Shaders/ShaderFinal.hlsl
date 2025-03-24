
Texture2D inputTex : register(t4);
SamplerState samLinear : register(s4);

cbuffer FinalQuad : register(b6)
{
    float ScaleX;
    float ScaleY;
    float OffsetX;
    float OffsetY;
}

struct VSInput
{
    float3 pos : POSITION;
    float2 uv  : TEXCOORD;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD;
};

PSInput mainVS(VSInput input)
{
    PSInput output;
    output.pos = float4(input.pos, 1.0f);
    output.pos.x *= ScaleX;
    output.pos.y *= ScaleY;
    output.pos.x += OffsetX;
    output.pos.y += OffsetY;
    
    output.uv = input.uv;
    return output;
}

float4 mainPS(PSInput input) : SV_TARGET
{
    return inputTex.Sample(samLinear, input.uv);
}
