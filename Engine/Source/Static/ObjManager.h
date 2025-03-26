#pragma once
#include "Container/Array.h"
#include "Container/Map.h"
#include "Container/String.h"
#include "Rendering/RendererDefine.h"
#include "CoreUObject/NameTypes.h"
#include "Core/Math/Vector.h"
#include "Core/Rendering/TextureLoader.h"

class  UStaticMesh;
struct FStaticMeshVertex;
struct FSubMesh;

// Cooked Data
struct FStaticMesh
{
    std::string PathFileName;

    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;
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
    FObjMaterialInfo()
        : Ns(1.0f)
        , Ka(1.0f, 1.0f, 1.0f)
        , Kd(1.0f, 1.0f, 1.0f)
        , Ks(1.0f, 1.0f, 1.0f)
        , Ke(0.0f, 0.0f, 0.0f)
        , Ni(1.f)
        , d(1.0f)
        , illum()
        , map_Ka(L"")
        , map_Kd(L"")
        , map_Ks(L"")
        , map_Ns(L"")
        , map_d(L"")
        , map_bump(L"")
        , map_refl(L"")
    {
    }

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
    static void SaveStaticMeshToBinary(const std::string& BinaryPath, FStaticMesh* Mesh);
    static FStaticMesh* LoadStaticMeshFromBinary(const std::string& BinaryPath);
    static void LoadMaterialTextures();
    static void MakeVertex(const TArray<float>& Vertex, const TArray<float>& Color, const TArray<float>& Normal, const TArray<float>& UV,
        FStaticMeshVertex& OutVertex)
    {
        OutVertex = {};
        OutVertex.Position = { Vertex[0], Vertex[1], Vertex[2] };
        OutVertex.Normal = { Normal[0], Normal[1], Normal[2] };
        OutVertex.UV = { UV[0], UV[1] };
        OutVertex.Color = { Color[0], Color[1], Color[2], 1};
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

    // MeshKey를 정규화하는 유틸리티 함수
    static FName GetNormalizedMeshKey(const FString& ObjPath)
    {
        // 파일 경로 전체에서 마지막 '.' 위치를 찾아 확장자 제거
        size_t dotPos = ObjPath.FindLastOf(TEXT("."));
        // 확장자 제거 후 남은 문자열(하위폴더 포함)을 key로 사용
        FString key = (dotPos == std::string::npos) ? ObjPath : ObjPath.Substr(0, dotPos);
        return FName(*key);
    }
    static void CalculateBoundingBox(const TArray<FStaticMeshVertex>& Vertices, FVector& OutMin, FVector& OutMax)
    {
        // 초기값 설정
        OutMin = { FLT_MAX, FLT_MAX, FLT_MAX };
        OutMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

        // 모든 정점 순회
        for (const FStaticMeshVertex& Vertex : Vertices)
        {
            OutMin.X = std::min(OutMin.X, Vertex.Position.X);
            OutMin.Y = std::min(OutMin.Y, Vertex.Position.Y);
            OutMin.Z = std::min(OutMin.Z, Vertex.Position.Z);

            OutMax.X = std::max(OutMax.X, Vertex.Position.X);
            OutMax.Y = std::max(OutMax.Y, Vertex.Position.Y);
            OutMax.Z = std::max(OutMax.Z, Vertex.Position.Z);
        }
    }
   

};


class FObjManager
{
public:
    static TMap<FName,FSubMesh> MaterialSubmeshMap;
    static TMap<FName,FObjMaterialInfo> MaterialMap;
    static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);
    static UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);
    static FObjImporter Importer;
  
private:
    static TMap<FString, FStaticMesh*> ObjStaticMeshMap;
};
