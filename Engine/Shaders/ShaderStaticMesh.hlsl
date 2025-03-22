// ShaderStaticMesh.hlsl

Texture2D TextureMap : register(t0);
SamplerState SampleType : register(s0);

cbuffer ChangeEveryObject : register(b0)
{
    matrix WorldMatrix;
    float4 CustomColor;
    uint bUseVertexColor;
}


cbuffer ChangeEveryFrame : register(b1)
{
    matrix ViewMatrix;
}

cbuffer ChangeOnResizeAndFov : register(b2)
{
    matrix ProjectionMatrix;
    float NearClip;
    float FarClip;
}

struct VS_INPUT
{
    float3 Position    : POSITION;
    float3 Normal : NORMAL;
    float3 Color  : COLOR0;
    float2 Tex    : TEXCOORD;
};

struct PS_INPUT
{
    float4 Position  : SV_POSITION;
    float4 Color : COLOR0;
    float2 Tex   : TEXCOORD0;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position.xyz, 1.0f);
    output.Position = mul(output.Position, WorldMatrix);
    output.Position = mul(output.Position, ViewMatrix);
    output.Position = mul(output.Position, ProjectionMatrix);
    output.Color = float4(input.Normal, 1.0f); 
    output.Color = float4(input.Color, 1.0f); 
    output.Tex   = input.Tex;   
    output.Color=CustomColor;
    //output.Color = bUseVertexColor != 0 ? input.Color : CustomColor;
    output.Tex = input.Tex;

    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    return float4(1,0,0,1);
    return input.Color;
    //float4 texColor = TextureMap.Sample(SampleType, input.Tex);
    //return texColor * input.Color;
}
