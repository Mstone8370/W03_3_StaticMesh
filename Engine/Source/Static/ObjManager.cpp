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
    FString BinaryPath = ObjPath + TEXT(".bin");
    FStaticMesh* StaticMesh = LoadStaticMeshFromBinary(BinaryPath.c_char());
    if (StaticMesh) {
        return StaticMesh;
    }
        
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
    StaticMesh = new FStaticMesh();
    StaticMesh->Vertices = CookedVertices;
    StaticMesh->Indices = CookedIndices;
    StaticMesh->PathFileName = ObjPath.c_char();
    StaticMesh->MaterialsName = MaterialsName;

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
    
    //binary로 저장
    SaveStaticMeshToBinary(BinaryPath.c_char(), StaticMesh);

    return StaticMesh;
}

// FStaticMesh를 바이너리 파일로 저장하는 함수
void FObjImporter::SaveStaticMeshToBinary(const std::string& BinaryPath, FStaticMesh* Mesh)
{
    std::ofstream out(BinaryPath.c_str(), std::ios::binary);
    if (!out)
    {
        return;
    }

    // 헤더 작성 ("SMB1": Static Mesh Binary 버전 1)
    const char header[4] = { 'S', 'M', 'B', '1' };
    out.write(header, sizeof(header));

    // 1. Vertices 저장
    uint32_t vertexCount = static_cast<uint32_t>(Mesh->Vertices.Num());
    out.write(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
    if (vertexCount > 0)
    {
        out.write(reinterpret_cast<const char*>(Mesh->Vertices.GetData()), vertexCount * sizeof(FStaticMeshVertex));
    }

    // 2. Indices 저장
    uint32_t indexCount = static_cast<uint32_t>(Mesh->Indices.Num());
    out.write(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
    if (indexCount > 0)
    {
        out.write(reinterpret_cast<const char*>(Mesh->Indices.GetData()), indexCount * sizeof(uint32_t));
    }

    // 3. MaterialsName 배열 저장
    uint32_t matCount = static_cast<uint32_t>(Mesh->MaterialsName.Num());
    out.write(reinterpret_cast<char*>(&matCount), sizeof(matCount));
    for (uint32_t i = 0; i < matCount; ++i)
    {
        std::string nameStr = Mesh->MaterialsName[i].ToString().c_char();
        uint32_t nameLen = static_cast<uint32_t>(nameStr.size());
        out.write(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        if (nameLen > 0)
        {
            out.write(nameStr.c_str(), nameLen);
        }
    }

    // 4. MaterialSubmeshMap 저장
    uint32_t submeshMapCount = static_cast<uint32_t>(FObjManager::MaterialSubmeshMap.Num());
    out.write(reinterpret_cast<char*>(&submeshMapCount), sizeof(submeshMapCount));
    for (auto& pair : FObjManager::MaterialSubmeshMap)
    {
        // 키(FName)를 문자열로 저장
        std::string keyStr = pair.Key.ToString().c_char();
        uint32_t keyLen = static_cast<uint32_t>(keyStr.size());
        out.write(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        if (keyLen > 0)
        {
            out.write(keyStr.c_str(), keyLen);
        }
        out.write(reinterpret_cast<const char*>(&pair.Value), sizeof(FSubMesh));
    }

    // 5. MaterialMap 저장 (전역 머티리얼 정보)
    uint32_t globalMatCount = static_cast<uint32_t>(FObjManager::MaterialMap.Num());
    out.write(reinterpret_cast<char*>(&globalMatCount), sizeof(globalMatCount));
    for (auto& pair : FObjManager::MaterialMap)
    {
        // 키(FName) 저장
        std::string keyStr = pair.Key.ToString().c_char();
        uint32_t keyLen = static_cast<uint32_t>(keyStr.size());
        out.write(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        if (keyLen > 0)
        {
            out.write(keyStr.c_str(), keyLen);
        }

        // 값(FObjMaterialInfo) 저장
        FObjMaterialInfo& info = pair.Value;
        out.write(reinterpret_cast<char*>(&info.Ns), sizeof(info.Ns));
        out.write(reinterpret_cast<char*>(&info.Ka), sizeof(info.Ka));
        out.write(reinterpret_cast<char*>(&info.Kd), sizeof(info.Kd));
        out.write(reinterpret_cast<char*>(&info.Ks), sizeof(info.Ks));
        out.write(reinterpret_cast<char*>(&info.Ke), sizeof(info.Ke));
        out.write(reinterpret_cast<char*>(&info.Ni), sizeof(info.Ni));
        out.write(reinterpret_cast<char*>(&info.d), sizeof(info.d));
        out.write(reinterpret_cast<char*>(&info.illum), sizeof(info.illum));
        // wide string 데이터를 저장하기 위한 람다 함수
        auto WriteWString = [&](const std::wstring& wstr) {
            uint32_t len = static_cast<uint32_t>(wstr.size());
            out.write(reinterpret_cast<char*>(&len), sizeof(len));
            if (len > 0)
            {
                out.write(reinterpret_cast<const char*>(wstr.data()), len * sizeof(wchar_t));
            }
            };
        WriteWString(info.map_Ka);
        WriteWString(info.map_Kd);
        WriteWString(info.map_Ks);
        WriteWString(info.map_Ns);
        WriteWString(info.map_d);
        WriteWString(info.map_bump);
        WriteWString(info.map_refl);
    }

    // 6. PathFileName 저장
    std::string pathStr = Mesh->PathFileName.c_str();
    uint32_t pathLen = static_cast<uint32_t>(pathStr.size());
    out.write(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
    if (pathLen > 0)
    {
        out.write(pathStr.c_str(), pathLen);
    }

    out.close();
}


// FStaticMesh를 바이너리 파일에서 로드하는 함수
FStaticMesh* FObjImporter::LoadStaticMeshFromBinary(const std::string& BinaryPath)
{
    std::ifstream in(BinaryPath.c_str(), std::ios::binary);
    if (!in)
    {
        return nullptr;
    }

    // 헤더 확인
    char header[4];
    in.read(header, sizeof(header));
    if (header[0] != 'S' || header[1] != 'M' || header[2] != 'B' || header[3] != '1')
    {
        in.close();
        return nullptr;
    }

    FStaticMesh* Mesh = new FStaticMesh();

    // 1. Vertices 로드
    uint32_t vertexCount;
    in.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
    Mesh->Vertices.Empty();
    Mesh->Vertices = TArray<FStaticMeshVertex>(vertexCount);
    if (vertexCount > 0)
    {
        in.read(reinterpret_cast<char*>(Mesh->Vertices.GetData()), vertexCount * sizeof(FStaticMeshVertex));
    }

    // 2. Indices 로드
    uint32_t indexCount;
    in.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
    Mesh->Indices.Empty();
    Mesh->Indices = TArray<uint32>(indexCount);
    if (indexCount > 0)
    {
        in.read(reinterpret_cast<char*>(Mesh->Indices.GetData()), indexCount * sizeof(uint32_t));
    }

    // 3. MaterialsName 배열 로드
    uint32_t matCount;
    in.read(reinterpret_cast<char*>(&matCount), sizeof(matCount));
    Mesh->MaterialsName.Empty();
    for (uint32_t i = 0; i < matCount; ++i)
    {
        uint32_t nameLen;
        in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string nameStr(nameLen, ' ');
        if (nameLen > 0)
        {
            in.read(&nameStr[0], nameLen);
        }
        Mesh->MaterialsName.Add(FName(nameStr.c_str()));
    }

    // 4. MaterialSubmeshMap 로드
    uint32_t submeshMapCount;
    in.read(reinterpret_cast<char*>(&submeshMapCount), sizeof(submeshMapCount));
    for (uint32_t i = 0; i < submeshMapCount; ++i)
    {
        uint32_t keyLen;
        in.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        std::string keyStr(keyLen, ' ');
        if (keyLen > 0)
        {
            in.read(&keyStr[0], keyLen);
        }
        FName keyFName(keyStr.c_str());
        FSubMesh submesh;
        in.read(reinterpret_cast<char*>(&submesh), sizeof(FSubMesh));
        FObjManager::MaterialSubmeshMap.Add(keyFName, submesh);
    }

    // 5. MaterialMap 로드 (전역 머티리얼 정보)
    uint32_t globalMatCount;
    in.read(reinterpret_cast<char*>(&globalMatCount), sizeof(globalMatCount));
    for (uint32_t i = 0; i < globalMatCount; ++i)
    {
        uint32_t keyLen;
        in.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));
        std::string keyStr(keyLen, ' ');
        if (keyLen > 0)
        {
            in.read(&keyStr[0], keyLen);
        }
        FName keyFName(keyStr.c_str());

        FObjMaterialInfo info;
        in.read(reinterpret_cast<char*>(&info.Ns), sizeof(info.Ns));
        in.read(reinterpret_cast<char*>(&info.Ka), sizeof(info.Ka));
        in.read(reinterpret_cast<char*>(&info.Kd), sizeof(info.Kd));
        in.read(reinterpret_cast<char*>(&info.Ks), sizeof(info.Ks));
        in.read(reinterpret_cast<char*>(&info.Ke), sizeof(info.Ke));
        in.read(reinterpret_cast<char*>(&info.Ni), sizeof(info.Ni));
        in.read(reinterpret_cast<char*>(&info.d), sizeof(info.d));
        in.read(reinterpret_cast<char*>(&info.illum), sizeof(info.illum));

        // 람다 함수로 wide string 읽기
        auto ReadWString = [&](std::wstring& wstr) {
            uint32_t len;
            in.read(reinterpret_cast<char*>(&len), sizeof(len));
            if (len > 0)
            {
                wstr.resize(len);
                in.read(reinterpret_cast<char*>(&wstr[0]), len * sizeof(wchar_t));
            }
            else
            {
                wstr.clear();
            }
            };
        ReadWString(info.map_Ka);
        ReadWString(info.map_Kd);
        ReadWString(info.map_Ks);
        ReadWString(info.map_Ns);
        ReadWString(info.map_d);
        ReadWString(info.map_bump);
        ReadWString(info.map_refl);
        if (!FObjManager::MaterialMap.Contains(keyFName))
        {
            FObjManager::MaterialMap.Add(keyFName, info);
        }

    }
    // 6. PathFileName 로드
    uint32_t pathLen;
    in.read(reinterpret_cast<char*>(&pathLen), sizeof(pathLen));
    std::string pathStr(pathLen, ' ');
    if (pathLen > 0)
    {
        in.read(&pathStr[0], pathLen);
    }
    Mesh->PathFileName = pathStr;

    in.close();
    LoadMaterialTextures();

    return Mesh;
}

void FObjImporter::LoadMaterialTextures()
{
    bool bLoaded = true;
    for (auto& material : FObjManager::MaterialMap) {
        if (material.Value.map_Ka.length())
            bLoaded |= UEngine::Get().LoadTexture(material.Key.ToString() + TEXT("map_Ka"), material.Value.map_Ka.c_str());

        if (material.Value.map_Kd.length())
            bLoaded |= UEngine::Get().LoadTexture(material.Key.ToString() + TEXT("map_Kd"), material.Value.map_Kd.c_str());

        if (material.Value.map_Ks.length())
            bLoaded |= UEngine::Get().LoadTexture(material.Key.ToString() + TEXT("map_Ks"), material.Value.map_Ks.c_str());

        if (material.Value.map_Ns.length())
            bLoaded |= UEngine::Get().LoadTexture(material.Key.ToString() + TEXT("map_Ns"), material.Value.map_Ns.c_str());

        if (material.Value.map_d.length())
            bLoaded |= UEngine::Get().LoadTexture(material.Key.ToString() + TEXT("map_d"), material.Value.map_d.c_str());

        if (material.Value.map_bump.length())
            bLoaded |= UEngine::Get().LoadTexture(material.Key.ToString() + TEXT("map_bump"), material.Value.map_bump.c_str());

        if (material.Value.map_refl.length())
            bLoaded |= UEngine::Get().LoadTexture(material.Key.ToString() + TEXT("map_refl"), material.Value.map_refl.c_str());
    }

}