#pragma once
#include "Container/Array.h"
#include "Container/Map.h"
#include "Container/String.h"
#include "Rendering/RendererDefine.h"
#include "CoreUObject/NameTypes.h"
#include "Core/Math/Vector.h"


class  UStaticMesh;
struct FStaticMeshVertex;
struct FSubMesh;

// Cooked Data
struct FStaticMesh
{
    std::string PathFileName;

    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;
    TArray<FSubMesh> SubMeshes;
    TArray<FName> MaterialsName;
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

//Raw Data
struct FObjInfo
{
    TArray<TArray<float>> Vertices;
    TArray<TArray<float>> Colors;
    TArray<TArray<float>> UVs;
    TArray<TArray<float>> Normals;
    TArray<uint32> VertexIndexList;
    TArray<uint32> UVIndexList;
    TArray<uint32> NormalIndexList;
    TMap<FName, FObjMaterialInfo> MaterialList;
    TArray<std::wstring> TextureList;
};


struct FSubMesh
{
    uint32 startIndex; // 전체 인덱스 버퍼에서 이 서브메시가 사용하는 시작 인덱스
    uint32 endIndex;   // 전체 인덱스 버퍼에서 이 서브메시가 사용하는 마지막 인덱스
};



// TMap<MaterialName, Tmap<StaticMeshName, TArray<SubMesh>>> t
// StaticMeshName은 Resources/..에 있지만 하위폴더가 존재가능하기 떄문에
// 파일 이름이 같아도 하위 폴더가 다르면 다른걸로 취급 하도록 StaticMeshName에 값을 저장해야함.


struct FObjImporter
{
    // Obj Parsing (*.obj to FObjInfo)
    // Material Parsing (*.obj to MaterialInfo)
    // Convert the Raw data to Cooked data (FStaticMesh)

    static FStaticMesh* BuildMeshFromObj(const FString& ObjPath);

    static void MakeVertex(const TArray<float>& Vertex, const TArray<float>& Color, const TArray<float>& Normal, const TArray<float>& UV,
        FStaticMeshVertex& OutVertex)
    {
        OutVertex = {};
        OutVertex.Position = { Vertex[0], Vertex[1], Vertex[2] };
        OutVertex.Color = { Color[0], Color[1], Color[2], 1};
        OutVertex.Normal = { Normal[0], Normal[1], Normal[2] };
        OutVertex.UV = { UV[0], UV[1] };
    }
    static void CalculateTangent(const FStaticMeshVertex& Vertex0, const FStaticMeshVertex& Vertex1,
        const FStaticMeshVertex& Vertex2, FVector& OutTangent)
    {
        float s1 = Vertex1.UV.X - Vertex0.UV.X;
        float t1 = Vertex1.UV.Y - Vertex0.UV.Y;
        float s2 = Vertex2.UV.X - Vertex0.UV.X;
        float t2 = Vertex2.UV.Y - Vertex0.UV.Y;
        float E1x = Vertex1.Position.X - Vertex0.Position.X;
        float E1y = Vertex1.Position.Y - Vertex0.Position.Y;
        float E1z = Vertex1.Position.Z - Vertex0.Position.Z;
        float E2x = Vertex2.Position.X - Vertex0.Position.X;
        float E2y = Vertex2.Position.Y - Vertex0.Position.Y;
        float E2z = Vertex2.Position.Z - Vertex0.Position.Z;
        float f = 1.f / (s1 * t2 - s2 * t1);
        float Tx = f * (t2 * E1x - t1 * E2x);
        float Ty = f * (t2 * E1y - t1 * E2y);
        float Tz = f * (t2 * E1z - t1 * E2z);

        OutTangent = FVector(Tx, Ty, Tz);
        OutTangent.Normalize();
    }
    static void UpdateMaterialSubmeshMap(const FString& ObjPath, const TArray<FName>& MaterialsName, const TArray<FSubMesh>& SubMeshes);
 
    // MeshKey를 정규화하는 유틸리티 함수
    static FName GetNormalizedMeshKey(const FString& ObjPath)
    {
        // 파일 경로에서 마지막 '/' 또는 '\' 위치를 찾음
        size_t pos = ObjPath.FindLastOf(TEXT("/\\"));
        // 마지막 구분자 이후의 문자열(파일 이름)을 추출 (구분자가 없으면 전체 문자열 사용)
        FString fileName = (pos == std::string::npos) ? ObjPath : ObjPath.Substr(pos + 1);
        // 파일 이름에서 마지막 '.' 위치를 찾아 확장자 제거 (점이 없으면 그대로 사용)
        size_t dotPos = fileName.FindLastOf(TEXT("."));
        fileName = (dotPos == std::string::npos) ? fileName : fileName.Substr(0, dotPos);
        return FName(*fileName);
    }

};


class FObjManager
{
public:
    static TMap<FName, TMap<FName, TArray<FSubMesh>>> MaterialSubmeshMap;
    static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);
    static UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);
    static FObjImporter Importer;
  
private:
    static TMap<FString, FStaticMesh*> ObjStaticMeshMap;
};
