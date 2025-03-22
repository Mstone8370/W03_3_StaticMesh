#include "pch.h"
#include "ObjManager.h"
#include "ObjectIterator.h"
#include "ObjectFactory.h"
#include "GameFrameWork/StaticMesh.h"
#include "Core/Container/Map.h"
#include "ObjReader.h"

TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;

FObjImporter FObjManager::Importer;
TMap<FName, TMap<FName, FSubMesh>> FObjManager::MaterialSubmeshMap;
FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
    if (FStaticMesh** FoundMesh = ObjStaticMeshMap.Find(PathFileName))
    {
        return *FoundMesh;
    }

    FStaticMesh* NewStaticMesh = Importer.BuildMeshFromObj(PathFileName);
    ObjStaticMeshMap.Add(PathFileName, NewStaticMesh);

    // 파일 경로를 기반으로 MeshKey 생성 ( 하위폴더 + 파일 이름 추출하는 로직 필요)
    FString MeshNameStr = PathFileName;
    FName MeshKey(*MeshNameStr);

    // Importer에서 머티리얼 목록(TMap)을 가져오고, 직접 구현한 TMap의 begin/end를 사용하여 키 배열 생성
    TMap<FName, FObjMaterialInfo> MaterialMap = Importer.GetMaterialList();
    TArray<FName> MaterialKeys;
    for (auto It = MaterialMap.begin(); It != MaterialMap.end(); ++It)
    {
        MaterialKeys.Add(It->Key);
    }

    // 각 서브메쉬에 대해 머티리얼 키 배열의 모든 머티리얼에 등록
    for (int32 i = 0; i < NewStaticMesh->SubMeshes.Num(); ++i)
    {
        FSubMesh CurrentSubMesh = NewStaticMesh->SubMeshes[i];

        for (const FName& MaterialName : MaterialKeys)
        {
            // 전역 맵에 해당 머티리얼 키가 없으면 새로 추가
            if (!MaterialSubmeshMap.Contains(MaterialName))
            {
                MaterialSubmeshMap.Add(MaterialName, TMap<FName, FSubMesh>());
            }

            TMap<FName,FSubMesh>& MeshMap = MaterialSubmeshMap[MaterialName];

            // MeshKey가 없으면 새로 생성
            if (!MeshMap.Contains(MeshKey))
            {
                MeshMap.Add(MeshKey,FSubMesh());
            }

            // 현재 서브메쉬 추가
            MeshMap[MeshKey] = CurrentSubMesh;
        }
    }

    // 디버깅용 전역맵 출력 예제
    for (auto MaterialIt = MaterialSubmeshMap.begin(); MaterialIt != MaterialSubmeshMap.end(); ++MaterialIt)
    {
        FName MaterialKey = MaterialIt->Key;
        OutputDebugString( MaterialKey.ToString().c_wchar());

        // 각 머티리얼에 대해 MeshMap을 순회
        TMap<FName, FSubMesh>& MeshMap = MaterialIt->Value;
        for (auto MeshIt = MeshMap.begin(); MeshIt != MeshMap.end(); ++MeshIt)
        {
            FName MeshKey = MeshIt->Key;
            std::wstring ws = L"Mesh : ";
            ws += MeshKey.ToString().c_wchar();
            ws += L"\n";
            OutputDebugString(ws.c_str());


            // 해당 MeshKey의 모든 서브메쉬 정보 출력 (예: startIndex, endIndex)
            FSubMesh& SubMesh = MeshIt->Value;

            ws = L"subMesh : ";
            ws += L"StartIndex: ";
            ws += std::to_wstring(SubMesh.startIndex);
            ws += L", ";
            ws += std::to_wstring(SubMesh.endIndex);
            ws += L"\n";
            OutputDebugString(ws.c_str());

        }
    }


    return NewStaticMesh;
}


UStaticMesh* FObjManager::LoadObjStaticMesh(const FString& PathFileName)
{   
    // TODO: TObjectIterator로 순환하여 이미 존재하는 경우 바로 리턴
    for (TObjectIterator<UStaticMesh> It; It; ++It)
    {
        UStaticMesh* StaticMesh = *It;
        if (StaticMesh->GetAssetPathFileName() == PathFileName)
            return *It;
    }

    FStaticMesh* StaticMeshAsset = LoadObjStaticMeshAsset(PathFileName);
    UStaticMesh* StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
    StaticMesh->SetStaticMeshAsset(StaticMeshAsset);
    return StaticMesh;
}

FStaticMesh* FObjImporter::BuildMeshFromObj(const FString& ObjPath)
{

    ObjReader Reader(ObjPath);
    FObjInfo RawData = Reader.GetRawData();
    MaterialList = Reader.GetMaterialList();
    SubMeshes = Reader.GetSubMeshes();

    // Cooked Data 생성: 총 인덱스 수는 VertexIndexList의 크기로 결정 (삼각형 기준)
    uint32 TotalIndices = RawData.VertexIndexList.Num();
    CookedVertices = TArray<FStaticMeshVertex>(TotalIndices);
    CookedIndices = TArray<uint32>(TotalIndices);

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
    FStaticMesh* StaticMesh = new FStaticMesh();
    StaticMesh->Vertices = CookedVertices;
    StaticMesh->Indices = CookedIndices;
    StaticMesh->PathFileName = ObjPath.c_char();
    StaticMesh->SubMeshes = SubMeshes;
    StaticMesh->MaterialsName = {};
    return StaticMesh;
}
