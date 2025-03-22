#pragma once
#include "Container/Array.h"
#include "Container/Map.h"
#include "Container/String.h"

/*
* struct FObjMaterialInfo와 vector include 테스트를 위한 임시입니다.
*/
#include "Core/Math/Vector.h"

class UStaticMesh;
struct FStaticMeshVertex;

struct FStaticMesh;
struct FObjInfo;
struct FObjMaterialInfo;
struct FObjImporter;

class FObjManager
{
public:
    static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);

    static UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);

private:
    static TMap<FString, FStaticMesh*> ObjStaticMeshMap;
};


// Cooked Data
struct FStaticMesh
{
    std::string PathFileName;

    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;
};

// Raw Data
struct FObjInfo
{
    // Vertex List
    // UV List
    // Normal List
    // Vertex Index List
    // UV Index List
    // Normal Index List
    // Material List
    // Texture List
};



struct FObjMaterialInfo
{
    float Ns;                // Specular exponent (광택 정도)
    FVector Ka;              // Ambient color 
    FVector Kd;              // Diffuse color 
    FVector Ks;              // Specular color 
    FVector Ke;              // Emissive color
    float Ni;                // Optical density 
    float d;                 // Dissolve (투명도; 1.0이면 불투명)
    float illum;             // Illumination model (조명 모델 번호)
    std::wstring map_Ka;     // Ambient texture map
    std::wstring map_Kd;     // Diffuse texture map
    std::wstring map_Ks;     // Specular texture map
    std::wstring map_Ns;     // Specular highlight map
    std::wstring map_d;      // Alpha (투명도) texture map
    std::wstring map_bump;   // Bump 또는 Normal map
    std::wstring map_refl;   // Reflection map
};
struct FSubMesh
{
    uint32 startIndex; // 전체 인덱스 버퍼에서 이 서브메시가 사용하는 시작 인덱스
    uint32 endIndex;   // 전체 인덱스 버퍼에서 이 서브메시가 사용하는 마지막 인덱스
    FString materialName; // 머티리얼 이름
};
struct FObjImporter
{
    // Obj Parsing (*.obj to FObjInfo)
    // Material Parsing (*.obj to MaterialInfo)
    // Convert the Raw data to Cooked data (FStaticMesh)
};