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
    float Ns; // Specular exponent (광택 정도)
    float d; // Dissolve (투명도; 1.0이면 불투명)
    float illum; // Illumination model (조명 모델 번호)
    float Ni; // Optical density 
    float3 Ka; // Ambient color 
    float pad0; // 패딩: float3 뒤에 float 1개 추가
    float3 Kd; // Diffuse color 
    float pad1;
    float3 Ks; // Specular color 
    float pad2;
    float3 Ke; // Emissive color
    float pad3;
}
cbuffer LightingConstants : register(b6)
{
    float3 LightDir;
    float pad4;
    float3 LightColor;
    float pad5;
    float3 AmbientColor;
    float pad6;
    float3 CameraPosition;
    float pad7;
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
    float3 WorldPos : TEXCOORD1; // 텍스처 좌표
};

////////
/// Vertex Shader
////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.Position = float4(input.Position, 1.0f);
    output.WorldPos = output.Position;
    
    output.Position = mul(output.Position, WorldMatrix);
    output.Position = mul(output.Position, ViewMatrix);
    output.Position = mul(output.Position, ProjectionMatrix);
    
    output.Normal = normalize(mul(input.Normal, (float3x3) NormalMatrix));
    output.Color = (bUseVertexColor == 1) ? float4(1, 1, 1, 1) : CustomColor;
    output.UV = input.UV;
    
    return output;
}

// 텍스처 샘플러 헬퍼 함수: index가 ActiveTextureCount 미만이면 샘플링, 아니면 검은색 반환.
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
///////
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

    // 머티리얼 컬러와 텍스처 결합 (Ka, Kd, Ks, Ke 값을 그대로 사용)
    float4 ambient = ambientTex * float4(Ka, 1.0);
    float4 diffuse = diffuseTex * float4(Kd, 1.0);
    float4 specular = specularTex * float4(Ks, 1.0);
    float4 emissive = float4(Ke, 1.0);
    
    // 모든 텍스처와 머티리얼 효과를 단순 합산
    float4 textureColor = diffuse + ambient + specular +
                          specularHighlightTex + bumpTex +
                          reflectionTex + emissive;
   
    // 법선 정규화
    float3 norm = normalize(input.Normal);
    
    // Diffuse 연산 (기존 방식)
    float diffIntensity = saturate(dot(norm, -LightDir));
    float3 ambientLight = AmbientColor * Ka;
    float3 diffuseLight = diffIntensity * LightColor * Kd;
    
    // 반사 기반 스펙큘러 연산
    // 반사 벡터 계산 (LightDir: 빛의 입사 방향)
    float3 reflectionLight = LightDir - (2.0 * norm * dot(LightDir, norm));
    reflectionLight = normalize(reflectionLight);
    
    // 카메라 위치와 픽셀의 월드 좌표를 이용해 뷰 벡터 계산
    float3 viewDir = normalize(CameraPosition - input.WorldPos);
    
    // 반사 벡터와 뷰 벡터 내적을 통해 스펙큘러 강도 산출
    float specValue = saturate(dot(reflectionLight, viewDir));
    
    // Ns(스펙큘러 지수)를 사용하여 스펙큘러 값 계산
    float spec = pow(specValue, Ns);
    spec = smoothstep(0.0, 1.0, spec);
    
    // Ks를 곱해 최종 스펙큘러 색상 산출 (LightColor 적용)
    float3 specularLight = LightColor * Ks * spec;
    
    // 최종 조명 연산: ambient, diffuse, 스펙큘러 항 합산
    float3 lighting = ambientLight + diffuseLight + specularLight;
    
    // 최종 색상에 조명 결과 적용 및 투명도 처리
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
