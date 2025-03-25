// 텍스처 배열과 샘플러 선언 (t0 ~ t6, s0 슬롯)
Texture2D MaterialTextures[7] : register(t1);
SamplerState MaterialSampler : register(s0);


////////
/// Constant Buffers
////////
cbuffer ChangeEveryObject : register(b0)
{
    matrix WorldMatrix;
    matrix NormalMatrix;
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

cbuffer UUIDColor : register(b3)
{
    float4 UUIDColor;
}

cbuffer MaterialInfo : register(b4)
{
    int ActiveTextureFlag;
    float Ns;               // Specular exponent (광택 정도)
    float d;                // Dissolve (투명도; 1.0이면 불투명)
    float illum;            // Illumination model (조명 모델 번호)
    float Ni;               // Optical density 
    float3 Ka;              // Ambient color 
    float3 Kd;              // Diffuse color 
    float3 Ks;              // Specular color 
    float3 Ke;              // Emissive color
    
}

////////
/// Input, Output Structures
////////
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Position : SV_POSITION; // 변환된 위치
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD0; // 텍스처 좌표
    float4 Color : COLOR; // 전달된 색상
};

////////
/// Vertex Shader
////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position, 1.0f);
    output.Position = mul(output.Position, WorldMatrix);
    output.Position = mul(output.Position, ViewMatrix);
    output.Position = mul(output.Position, ProjectionMatrix);
    
    output.Normal = normalize(mul(input.Normal, (float3x3) NormalMatrix));
    output.Color = (bUseVertexColor == 1) ? float4(1, 1, 1, 1) : CustomColor;
    output.UV = input.UV;
    
    return output;
}

// 텍스처 샘플러 헬퍼 함수: index가 ActiveTextureCount 미만이면 샘플링, 아니면 기본 흰색 반환.
float4 SampleTexture(int index, float2 uv)
{
    if (ActiveTextureFlag & (1 << (index - 1)))
    {
        return MaterialTextures[index - 1].Sample(MaterialSampler, uv);
    }
    return float4(0, 0, 0, 1);
}


////////
/// Pixel Shader (텍스처 매핑, 조명 연산)
////////
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    if (ActiveTextureFlag == 0)
    {
        return (bUseVertexColor == 1) ? input.Color : CustomColor;
    }
    
    // 각 텍스처 샘플링 결과
    float4 ambientTex = SampleTexture(1, input.UV);
    float4 diffuseTex = SampleTexture(2, input.UV);
    float4 specularTex = SampleTexture(3, input.UV);
    float4 specularHighlightTex = SampleTexture(4, input.UV);
    float4 dTex = SampleTexture(5, input.UV);
    float4 bumpTex = SampleTexture(6, input.UV);
    float4 reflectionTex = SampleTexture(7, input.UV);

    // MaterialInfo에 정의된 머티리얼 컬러 값들을 텍스처에 곱하여 각 성분 구성
    float4 ambient = ambientTex * float4(Ka, 1.0);
    float4 diffuse = diffuseTex * float4(Kd, 1.0);
    float4 specular = specularTex * float4(Ks, 1.0);
    float4 emissive = float4(Ke, 1.0);

    // 각 텍스처와 머티리얼 값들을 가중치로 혼합
    float4 textureColor = diffuse * 0.5 +
                          ambient * 0.1 +
                          specular * 0.1 +
                          specularHighlightTex * 0.1 +
                          bumpTex * 0.05 +
                          reflectionTex * 0.8 +
                          emissive;
    
     // 하드코딩된 흰색 조명 적용
    float3 gLightDir = normalize(float3(-1.0, 0.0, 1.0)); // 빛의 방향 (위쪽과 약간 전방)
    float3 gLightColor = float3(1.0, 1.0, 1.0); // 조명 색상: 흰색
    float3 gAmbientColor = float3(0.1, 0.1, 0.1); // 주변광

    // 노멀 벡터 정규화
    float3 norm = normalize(input.Normal);
    
    // Diffuse 조명 계산: 노멀과 빛 방향의 내적
    float diff = saturate(dot(norm, -gLightDir));
    
    // 그림자 효과를 위한 shadow factor 계산 (내적 값에 따라 부드럽게 전환)
    float shadowFactor = smoothstep(0.3, 0.6, diff);
    
    // Ambient 조명 (머티리얼의 Ka 값 반영)
    float3 ambientLight = gAmbientColor * Ka;
    
    // Diffuse 조명: 내적값에 shadow factor를 곱해 조명 강도를 조절 (강조 효과)
    float3 diffuseLight = diff * gLightColor * Kd * (0.2 + 1.0 * shadowFactor);
    
    // 최종 조명 결과: Ambient + Diffuse
    float3 lighting = ambientLight + diffuseLight;
    
    // 텍스처 컬러에 조명 효과 적용
    float4 finalColor = float4(textureColor.rgb * lighting, textureColor.a);
    
    finalColor.a *= d;
   

    return finalColor;
}


////////
/// 픽킹 픽셀 쉐이더 (UUID 색상 출력)
////////
float4 PickingPS(PS_INPUT input) : SV_TARGET
{
    return UUIDColor;
}
