#include "pch.h"
#include "ObjReader.h"
#include "Engine/Engine.h"
#include <sstream>
#include <filesystem>

ObjReader::ObjReader()
{
    FilePath = FString("");
}

ObjReader::ObjReader(const FString& InFilePath)
{
    SetFilePath(InFilePath);
}

ObjReader::~ObjReader()
{
    Clear();
}

TArray<float> ObjReader::GetVertex(int32 Idx)
{
    if (0 <= Idx && Idx < RawData.Vertices.Num())
    {
        return RawData.Vertices[Idx];
    }
    return {};
}

TArray<float> ObjReader::GetNormal(int32 Idx)
{
    if (0 <= Idx && Idx < RawData.Normals.Num())
    {
        return RawData.Normals[Idx];
    }
    return {};
}

TArray<float> ObjReader::GetUV(int32 Idx)
{
    if (0 <= Idx && Idx < RawData.UVs.Num())
    {
        return RawData.UVs[Idx];
    }
    return {};
}

TArray<uint32> ObjReader::GetVertexIndices()
{
    TArray<uint32> Indices(Faces.Num() * 3);
    int Cnt = 0;
    for (TArray<TArray<uint32>>& Face : Faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            Indices[Cnt] = Face[i][0];
            ++Cnt;
        }
    }
    return Indices;
}
TArray<uint32> ObjReader::GetUVIndices()
{
    TArray<uint32> Indices(Faces.Num() * 3);
    int Cnt = 0;
    for (TArray<TArray<uint32>>& Face : Faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            Indices[Cnt] = Face[i][1];  // UV 인덱스
            ++Cnt;
        }
    }
    return Indices;
}

TArray<uint32> ObjReader::GetNormalIndices()
{
    TArray<uint32> Indices(Faces.Num() * 3);
    int Cnt = 0;
    for (TArray<TArray<uint32>>& Face : Faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            Indices[Cnt] = Face[i][2];  // 노멀 인덱스
            ++Cnt;
        }
    }
    return Indices;
}

FObjInfo ObjReader::GetRawData()
{
    RawData.VertexIndexList = GetVertexIndices();
    RawData.NormalIndexList = GetNormalIndices();
    RawData.UVIndexList = GetUVIndices();
   
    return RawData;
}

void ObjReader::SetFilePath(const FString& InFilePath)
{
    if (CheckFile(InFilePath))
    {
        FilePath = InFilePath;
        ReadFile();
        ReadMaterialFile();
        CreateSubMesh();
        LoadMaterialTextures();
    }
}

void ObjReader::Clear()
{
    for (TArray<float>& Vertex : RawData.Vertices)
    {
        Vertex.Empty();
    }
    RawData.Vertices.Empty();

    for (TArray<float>& Normal : RawData.Normals)
    {
        Normal.Empty();
    }
    RawData.Normals.Empty();

    for (TArray<float>& UV : RawData.UVs)
    {
        UV.Empty();
    }
    RawData.UVs.Empty();

    for (TArray<TArray<uint32>>& Face : Faces)
    {
        for (int i = 0; i < 3; ++i)
        {
            Face[i].Empty();
        }
        Face.Empty();
    }
    Faces.Empty();
}

bool ObjReader::CheckFile(const FString& InFilePath) const
{
    std::string path(InFilePath.c_char());
    std::ifstream f(path);
    return f.good();
}

void ObjReader::ReadFile()
{
    if (!File.is_open())
    {
        File.open(*FilePath);
    }

    /**
     * 블렌더에서 Export 시 공통 설정
     *   Geometry
     *     ✅ Triangulated Mesh
     *
     * 방식에 따른 Export 옵션
     *   1. 언리얼 엔진 방식 (Yaw를 -90도로 맞추어야 X축 방향에 Forward가 맞춰짐)
     *     General
     *       Forward Axis:  Y
     *       Up Axis:       Z
     *       
     *   2. 기본으로 X축 방향에 Forward가 맞춰지는 방식
     *     General
     *       Forward Axis: -X
     *       Up Axis:       Z
     */

    std::string Line; // std::string을 참조해야하므로, FString 대신 std::string 사용
   
    FString CurrentMaterial;
    while (std::getline(File, Line))
    {
        if (Line.empty())
            continue;
        std::stringstream TokenStream(Line);
        TArray<std::string> Tokens;
        std::string Token;
        while (std::getline(TokenStream, Token, ' '))
        {
            Tokens.Add(Token);
        }
        if (Tokens.IsEmpty()) continue;
        const std::string& Key = Tokens[0];
        if (Key == "mtllib")
        {
            std::filesystem::path basePath(*FilePath);
            basePath = basePath.parent_path();
            // Tokens[1]에 포함된 하위 폴더도 함께 고려하여 전체 경로 생성
            MaterialPath = (basePath / Tokens[1]).generic_string();
        }
        else if (Key == "v")
        {
            TArray<float> Vertex(3);
            Vertex[0] = std::stof(Tokens[1]);  // Location X
            Vertex[1] = -std::stof(Tokens[2]); // Location Y
            Vertex[2] = std::stof(Tokens[3]);  // Location Z

            TArray<float> Color(3);
            if (Tokens.Num() > 4) {  // Vertex Color
                Color[0] = std::stof(Tokens[4]);
                Color[1] = std::stof(Tokens[5]);
                Color[2] = std::stof(Tokens[6]);
            }
            RawData.Colors.Add(Color);
            RawData.Vertices.Add(Vertex);
        }
        else if (Key == "vn")
        {
            TArray<float> Normal(3);
            Normal[0] = std::stof(Tokens[1]);  // Normal X
            Normal[1] = -std::stof(Tokens[2]); // Normal Y
            Normal[2] = std::stof(Tokens[3]);  // Normal Z
            RawData.Normals.Add(Normal);
        }
        else if (Key == "vt")
        {
            TArray<float> UV(2);
            UV[0] = std::stof(Tokens[1]);       // U
            UV[1] = 1.f - std::stof(Tokens[2]); // V; Obj 파일은 오른손 좌표계 기준이므로, 왼손 좌표계의 UV맵 좌표로 변경
            RawData.UVs.Add(UV);
        }
        else if (Key == "usemtl") 
        {
            CurrentMaterial = FString(Tokens[1]);
        }
        else if (Key == "f")
        {
            TArray<TArray<uint32>> Face(3);
            for (int i = 0; i < 3; ++i)
            {
                Face[i] = TArray<uint32>(3);
                std::stringstream ss(Tokens[i + 1]);
                std::string Val;
                int Cnt = 0;
                while (std::getline(ss, Val, '/'))
                {
                    Face[i][Cnt] = std::stoi(Val) - 1;
                    ++Cnt;
                }
            }
            Faces.Add(Face);
            if (!MaterialIndexMap.Contains(CurrentMaterial))
            {
                MaterialIndexMap.Add(CurrentMaterial, TArray<uint32>());
            }
            // 각 face의 첫번째 값(정점 인덱스)을 머티리얼별로 저장
            MaterialIndexMap[CurrentMaterial].Add(Face[0][0]);
            MaterialIndexMap[CurrentMaterial].Add(Face[1][0]);
            MaterialIndexMap[CurrentMaterial].Add(Face[2][0]);
        }
    }
    File.close();
}

void ObjReader::ReadMaterialFile()
{
    if (MaterialPath.IsEmpty()) return;

    std::wifstream In(MaterialPath.c_char());
    if (!In) {
        OutputDebugString(TEXT("\nError!! Failed open material File!!\n"));
        return;
    }
    In.imbue(std::locale("ko_KR.UTF-8")); //한글 깨짐 문제로 한국 로케일.

    std::wstring Line;
    FString CurrentMaterial;
    FObjMaterialInfo ObjMaterialInfo;

    std::filesystem::path basePath(*MaterialPath);
    basePath = basePath.parent_path();

    while (std::getline(In, Line))
    {
        if (Line.empty())
            continue;

        std::wstringstream TokenStream(Line);
        TArray<std::wstring> Tokens;
        std::wstring Token;
        while (std::getline(TokenStream, Token, L' '))
        {
            Tokens.Add(Token);
        }

        if (Tokens.IsEmpty()) continue;
        const std::wstring& Key = Tokens[0];
        if (Key == TEXT("newmtl")) {
            CurrentMaterial = FString(Tokens[1]);
            ObjMaterialInfo = FObjMaterialInfo();
        }
        else if (Key == TEXT("Ns")) {
            ObjMaterialInfo.Ns = std::stof(Tokens[1]);
        }
        else if (Key == TEXT("Ka")) {
            ObjMaterialInfo.Ka = { std::stof(Tokens[1]), std::stof(Tokens[2]), std::stof(Tokens[3]) };
        }
        else if (Key == TEXT("Kd")) {
            ObjMaterialInfo.Kd = { std::stof(Tokens[1]), std::stof(Tokens[2]), std::stof(Tokens[3]) };
        }
        else if (Key == TEXT("Ks")) {
            ObjMaterialInfo.Ks = { std::stof(Tokens[1]), std::stof(Tokens[2]), std::stof(Tokens[3]) };
        }
        else if (Key == TEXT("Ke")) {
            ObjMaterialInfo.Ke = { std::stof(Tokens[1]), std::stof(Tokens[2]), std::stof(Tokens[3]) };
        }
        else if (Key == TEXT("Ni")) {
            ObjMaterialInfo.Ni = std::stof(Tokens[1]);
        }
        else if (Key == TEXT("d")) {
            ObjMaterialInfo.d = std::stof(Tokens[1]);
        }
        else if (Key == TEXT("illum")) {
            ObjMaterialInfo.illum = std::stof(Tokens[1]);
        }
        else if (Key == TEXT("map_Ka")) {
            ObjMaterialInfo.map_Ka = ExtractFileName(Tokens[1]);
        }
        else if (Key == TEXT("map_Kd")) {
            ObjMaterialInfo.map_Kd = ExtractFileName(Tokens[1]);
        }
        else if (Key == TEXT("map_Ks")) {
            ObjMaterialInfo.map_Ks = ExtractFileName(Tokens[1]);
        }
        else if (Key == TEXT("map_Ns")) {
            ObjMaterialInfo.map_Ns = ExtractFileName(Tokens[1]);
        }
        else if (Key == TEXT("map_d")) {
            ObjMaterialInfo.map_d = ExtractFileName(Tokens[1]);
        }
        else if (Key == TEXT("map_bump")) {
            ObjMaterialInfo.map_bump = ExtractFileName(Tokens[1]);
        }
        else if (Key == TEXT("map_refl")) {
            ObjMaterialInfo.map_refl = ExtractFileName(Tokens[1]);
        }
        FObjManager::MaterialMap[CurrentMaterial] = ObjMaterialInfo;
    }

}

void ObjReader::CreateSubMesh()
{
    int32 currentStartIndex = 0;
    
    // MaterialIndexMap에 기록된 내용을 활용하여 SubMesh 생성
    for (auto& pair : MaterialIndexMap)
    {
        FSubMesh Submesh;
        TArray<uint32>& indices = pair.Value;
        int32 count = indices.Num();

        Submesh.startIndex = currentStartIndex;
        Submesh.endIndex = currentStartIndex + count - 1;
        SubMeshes.Add(Submesh);

        if (!MaterialsName.Contains(pair.Key)) {
            MaterialsName.Add(pair.Key);
        }
        currentStartIndex += count;
    }
}

void ObjReader::LoadMaterialTextures()
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
