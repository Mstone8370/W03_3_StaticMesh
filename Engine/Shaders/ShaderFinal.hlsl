
Texture2D inputTex : register(t0);
SamplerState samLinear : register(s0);

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
    output.uv = input.uv;
    return output;
}

float4 mainPS(PSInput input) : SV_TARGET
{
    return float4(0.5f, 0.5f, 0.5f, 1.0f);
    return inputTex.Sample(samLinear, input.uv);
}
