#include "pch.h"
#include "ObjManager.h"
#include "ObjectIterator.h"
#include "ObjectFactory.h"
#include "GameFrameWork/StaticMesh.h"
#include "ObjReader.h"

TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
    if(FStaticMesh** It = ObjStaticMeshMap.Find(PathFileName))
    {
        return *It;
    }
    
    // TODO: OBJ Parsing and create a new FStaticMesh
    FStaticMesh* NewStaticMesh = nullptr;
    ObjStaticMeshMap.Add(PathFileName, NewStaticMesh);
    return NewStaticMesh;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(const FString& PathFileName)
{   
    // TODO: TObjectIterator로 순환하여 이미 존재하는 경우 바로 리턴
    /*
    for (TObjectIterator<UStaticMesh> It; It; ++It)
    {
        UStaticMesh* StaticMesh = *It;
        if (StaticMesh->GetAssetPathFileName() == PathFileName)
            return *It;
    }
    */

    FStaticMesh* StaticMeshAsset = LoadObjStaticMeshAsset(PathFileName);
    UStaticMesh* StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
    StaticMesh->SetStaticMeshAsset(StaticMeshAsset);
    return StaticMesh;
}

bool FObjImporter::BuildMeshFromObj(const FString& ObjPath)
{

    ObjReader Reader(ObjPath);
    FObjInfo RawData = Reader.GetRawData();

    SubMeshes = Reader.GetSubMeshes();

    // Cooked Data 생성: 총 인덱스 수는 VertexIndexList의 크기로 결정 (삼각형 기준)
    uint32 TotalIndices = RawData.VertexIndexList.Num();
    Vertices = TArray<FStaticMeshVertex>(TotalIndices);
    Indices = TArray<uint32>(TotalIndices);

    // 각 삼각형 단위로 정점 생성
    uint32 VertexCount = 0;
    for (uint32 i = 0; i < TotalIndices; i += 3)
    {
        // 정점 인덱스 추출
        uint32 vIdx0 = RawData.VertexIndexList[i];
        uint32 vIdx1 = RawData.VertexIndexList[i + 1];
        uint32 vIdx2 = RawData.VertexIndexList[i + 2];

        // UV 인덱스 추출
        uint32 uvIdx0 = RawData.UVIndexList[i];
        uint32 uvIdx1 = RawData.UVIndexList[i + 1];
        uint32 uvIdx2 = RawData.UVIndexList[i + 2];

        // 노멀 인덱스 추출
        uint32 nIdx0 = RawData.NormalIndexList[i];
        uint32 nIdx1 = RawData.NormalIndexList[i + 1];
        uint32 nIdx2 = RawData.NormalIndexList[i + 2];

        // 각 인덱스를 사용해 실제 데이터를 가져옴
        TArray<float> Pos0 = RawData.Vertices[vIdx0];
        TArray<float> UV0 = RawData.UVs[uvIdx0];
        TArray<float> Norm0 = RawData.Normals[nIdx0];

        TArray<float> Pos1 = RawData.Vertices[vIdx1];
        TArray<float> UV1 = RawData.UVs[uvIdx1];
        TArray<float> Norm1 = RawData.Normals[nIdx1];

        TArray<float> Pos2 = RawData.Vertices[vIdx2];
        TArray<float> UV2 = RawData.UVs[uvIdx2];
        TArray<float> Norm2 = RawData.Normals[nIdx2];

        // 각 정점 생성
        FStaticMeshVertex Vertex0, Vertex1, Vertex2;
        MakeVertex(Pos0, Norm0, UV0, Vertex0);
        MakeVertex(Pos1, Norm1, UV1, Vertex1);
        MakeVertex(Pos2, Norm2, UV2, Vertex2);

        // Tangent 계산 (필요에 따라 좌표계 변환 로직 포함)
        CalculateTangent(Vertex0, Vertex1, Vertex2, Vertex0.Tangent);
        CalculateTangent(Vertex1, Vertex2, Vertex0, Vertex1.Tangent);
        CalculateTangent(Vertex2, Vertex0, Vertex1, Vertex2.Tangent);

        // Cooked 데이터 저장
        Vertices[VertexCount] = Vertex0;
        Indices[VertexCount] = VertexCount;
        ++VertexCount;

        Vertices[VertexCount] = Vertex1;
        Indices[VertexCount] = VertexCount;
        ++VertexCount;

        Vertices[VertexCount] = Vertex2;
        Indices[VertexCount] = VertexCount;
        ++VertexCount;
    }

    // 필요 시 SubMesh 정보나 Material 정보 출력 처리
    for (int i = 0; i < SubMeshes.Num(); ++i)
    {
        FSubMesh& sm = SubMeshes[i];
        FObjMaterialInfo material = RawData.MaterialList[sm.materialName];

        std::wstring ws = sm.materialName.ToString().c_wchar();
        std::wstring output = L"\nSubMesh Material:" + ws + L":\n";
        output += TEXT(" StartIndices ") + std::to_wstring(sm.startIndex) + L":\n";
        output += TEXT(" EndIndices ") + std::to_wstring(sm.endIndex) + L":\n";
        output += TEXT("Material Info:\n");
        output += TEXT("  Ns: ") + std::to_wstring(material.Ns) + L"\n";
        output += TEXT("  Ka: (") + std::to_wstring(material.Ka.X) + L", " + std::to_wstring(material.Ka.Y) + L", " + std::to_wstring(material.Ka.Z) + L")\n";
        output += TEXT("  Kd: (") + std::to_wstring(material.Kd.X) + L", " + std::to_wstring(material.Kd.Y) + L", " + std::to_wstring(material.Kd.Z) + L")\n";
        output += TEXT("  Ks: (") + std::to_wstring(material.Ks.X) + L", " + std::to_wstring(material.Ks.Y) + L", " + std::to_wstring(material.Ks.Z) + L")\n";
        output += TEXT("  Ke: (") + std::to_wstring(material.Ke.X) + L", " + std::to_wstring(material.Ke.Y) + L", " + std::to_wstring(material.Ke.Z) + L")\n";
        output += TEXT("  Ni: ") + std::to_wstring(material.Ni) + L"\n";
        output += TEXT("  d: ") + std::to_wstring(material.d) + L"\n";
        output += TEXT("  illum: ") + std::to_wstring(material.illum) + L"\n";
        output += TEXT("  map_Ka: ") + material.map_Ka + L"\n";
        output += TEXT("  map_Kd: ") + material.map_Kd + L"\n";
        output += TEXT("  map_Ks: ") + material.map_Ks + L"\n";
        output += TEXT("  map_Ns: ") + material.map_Ns + L"\n";
        output += TEXT("  map_d: ") + material.map_d + L"\n";
        output += TEXT("  map_bump: ") + material.map_bump + L"\n";
        output += TEXT("  map_refl: ") + material.map_refl + L"\n";

        OutputDebugString(output.c_str());
    }

    return true;
}
