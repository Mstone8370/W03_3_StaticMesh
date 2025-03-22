#pragma once
#include "Container/Array.h"
#include "Container/Map.h"
#include "Container/String.h"
#include "Rendering/RendererDefine.h"
#include "CoreUObject/NameTypes.h"
#include "Core/Math/Vector.h"


class  UStaticMesh;
struct FStaticMeshVertex;
struct FStaticMesh;
struct FObjImporter;
struct FObjInfo;

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
    FName materialName; // 머티리얼 이름
};



struct FObjImporter
{
    // Obj Parsing (*.obj to FObjInfo)
    // Material Parsing (*.obj to MaterialInfo)
    // Convert the Raw data to Cooked data (FStaticMesh)
    uint32 GetVertexNum() const { return Vertices.Num(); }
    uint32 GetIndexNum() const { return Indices.Num(); }

    TArray<FStaticMeshVertex> GetVertices() { return Vertices; }
    TArray<uint32> GetIndices() { return Indices; }
    
    TArray<FStaticMeshVertex> Vertices;  
    TArray<uint32> Indices;
    
    uint32 VerticesNum;
    uint32 IndicesNum;

    
    TArray<FSubMesh> SubMeshes;


    bool BuildMeshFromObj(const FString& ObjPath);
    void MakeVertex(const TArray<float>& Vertex, const TArray<float>& Normal, const TArray<float>& UV,
        FStaticMeshVertex& OutVertex)
    {
        OutVertex = {};
        OutVertex.Position = { Vertex[0], Vertex[1], Vertex[2] };
        OutVertex.Normal = { Normal[0], Normal[1], Normal[2] };
        OutVertex.UV = { UV[0], UV[1] };
    }
    void CalculateTangent(const FStaticMeshVertex& Vertex0, const FStaticMeshVertex& Vertex1,
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

};