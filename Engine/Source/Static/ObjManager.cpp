#include "pch.h"
#include "ObjManager.h"
#include "ObjectIterator.h"
#include "ObjectFactory.h"
#include "GameFrameWork/StaticMesh.h"
#include "Core/Container/Map.h"
#include "ObjReader.h"

// 전역 변수들
TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;
TMap<FName, FObjMaterialInfo> FObjManager::MaterialMap;
FObjImporter FObjManager::Importer;

// MaterialSubmeshMap: 머티리얼 이름 -> 서브메쉬 
TMap<FName, FSubMesh> FObjManager::MaterialSubmeshMap;

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
    if (FStaticMesh** FoundMesh = ObjStaticMeshMap.Find(PathFileName))
    {
        return *FoundMesh;
    }

    FStaticMesh* NewStaticMesh = Importer.BuildMeshFromObj(PathFileName);
    ObjStaticMeshMap.Add(PathFileName, NewStaticMesh);
    return NewStaticMesh;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(const FString& PathFileName)
{
    for (TObjectIterator<UStaticMesh> It; It; ++It)
    {
        UStaticMesh* StaticMesh = *It;
        if (StaticMesh->GetAssetPathFileName() == PathFileName) {
            return *It;
        }
    }

    FStaticMesh* StaticMeshAsset = LoadObjStaticMeshAsset(PathFileName);
    UStaticMesh* StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
    StaticMesh->SetStaticMeshAsset(StaticMeshAsset);
    return StaticMesh;
}

FStaticMesh* FObjImporter::BuildMeshFromObj(const FString& ObjPath)
{
    TArray<FSubMesh> SubMeshes;
    
    TArray<FStaticMeshVertex> CookedVertices;
    TArray<uint32> CookedIndices;

    ObjReader Reader(ObjPath);
    
    FObjInfo RawData = Reader.GetRawData();
      
    SubMeshes = Reader.GetSubMeshes();

    uint32 TotalIndices = RawData.VertexIndexList.Num();
    CookedVertices = TArray<FStaticMeshVertex>(TotalIndices);
    CookedIndices = TArray<uint32>(TotalIndices);

    uint32 VertexCount = 0;
    
    for (uint32 i = 0; i < TotalIndices; i += 3)
    {
        uint32 vIdx0 = RawData.VertexIndexList[i];
        uint32 vIdx1 = RawData.VertexIndexList[i + 1];
        uint32 vIdx2 = RawData.VertexIndexList[i + 2];

        uint32 uvIdx0 = RawData.UVIndexList[i];
        uint32 uvIdx1 = RawData.UVIndexList[i + 1];
        uint32 uvIdx2 = RawData.UVIndexList[i + 2];

        uint32 nIdx0 = RawData.NormalIndexList[i];
        uint32 nIdx1 = RawData.NormalIndexList[i + 1];
        uint32 nIdx2 = RawData.NormalIndexList[i + 2];

        TArray<float> Pos0 = RawData.Vertices[vIdx0];
        TArray<float> Col0 = RawData.Colors[vIdx0];
        TArray<float> UV0 = RawData.UVs[uvIdx0];
        TArray<float> Norm0 = RawData.Normals[nIdx0];

        TArray<float> Pos1 = RawData.Vertices[vIdx1];
        TArray<float> Col1 = RawData.Colors[vIdx1];
        TArray<float> UV1 = RawData.UVs[uvIdx1];
        TArray<float> Norm1 = RawData.Normals[nIdx1];

        TArray<float> Pos2 = RawData.Vertices[vIdx2];
        TArray<float> Col2 = RawData.Colors[vIdx2];
        TArray<float> UV2 = RawData.UVs[uvIdx2];
        TArray<float> Norm2 = RawData.Normals[nIdx2];
     
        FStaticMeshVertex Vertex0, Vertex1, Vertex2;
        MakeVertex(Pos0, Col0, Norm0, UV0, Vertex0);
        MakeVertex(Pos2, Col2, Norm2, UV2, Vertex1);
        MakeVertex(Pos1, Col1, Norm1, UV1, Vertex2);


        CalculateTangent(Vertex0, Vertex1, Vertex2, Vertex0.Tangent);
        CalculateTangent(Vertex1, Vertex2, Vertex0, Vertex1.Tangent);
        CalculateTangent(Vertex2, Vertex0, Vertex1, Vertex2.Tangent);

        CookedVertices[VertexCount] = Vertex0;
        CookedIndices[VertexCount] = VertexCount;
        ++VertexCount;

        CookedVertices[VertexCount] = Vertex1;
        CookedIndices[VertexCount] = VertexCount;
        ++VertexCount;

        CookedVertices[VertexCount] = Vertex2;
        CookedIndices[VertexCount] = VertexCount;
        ++VertexCount;
    }
    

    // 파싱 단계에서 얻은 머티리얼 이름 배열 (usemtl 지시어에 의해 추출)
    TArray<FName> MaterialsName = Reader.GetMaterialsName();
    FStaticMesh* StaticMesh = new FStaticMesh();
    StaticMesh->Vertices = CookedVertices;
    StaticMesh->Indices = CookedIndices;
    StaticMesh->PathFileName = ObjPath.c_char();
    StaticMesh->MaterialsName = MaterialsName;

    FName MeshKey = GetNormalizedMeshKey(ObjPath);

    //머티리얼 하나당 하나의 서브메쉬가 나온다.
    for (int i = 0; i < MaterialsName.Num(); ++i)
    {
        const FName& MaterialName = MaterialsName[i];
        const FSubMesh& CurrentSubMesh = SubMeshes[i];
        if (!FObjManager::MaterialSubmeshMap.Contains(MaterialName))
        {
            FObjManager::MaterialSubmeshMap[MaterialName] = CurrentSubMesh;
        }
    }

    return StaticMesh;
}