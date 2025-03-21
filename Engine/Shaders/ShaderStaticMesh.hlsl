// ShaderStaticMesh.hlsl

Texture2D TextureMap : register(t0);
SamplerState SampleType : register(s0);

cbuffer ObjectConstants : register(b0)
{
    float4x4 World;
    float4 ObjectColor;
    int bUseVertexColor;
    float3 Padding;
};

cbuffer ViewProjection : register(b1)
{
    float4x4 ViewProj;
};

struct VSInput
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
    float4 Color  : COLOR0;
    float2 Tex    : TEXCOORD0;
};

struct PSInput
{
    float4 PosH  : SV_POSITION;
    float4 Color : COLOR0;
    float2 Tex   : TEXCOORD0;
};

PSInput mainVS(VSInput input)
{
    PSInput output;

    float4 worldPos = mul(float4(input.Pos, 1.0f), World);
    output.PosH = mul(worldPos, ViewProj);

    output.Color = bUseVertexColor != 0 ? input.Color : ObjectColor;
    output.Tex = input.Tex;

    return output;
}

float4 mainPS(PSInput input) : SV_TARGET
{
    float4 texColor = TextureMap.Sample(SampleType, input.Tex);
    return texColor * input.Color;
}
