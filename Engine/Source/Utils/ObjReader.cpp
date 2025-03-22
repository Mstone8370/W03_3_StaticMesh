#include "pch.h"
#include "ObjReader.h"
#include <sstream>

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

TArray<float> ObjReader::GetVertexColor(int32 Idx)
{
    if (0 <= Idx && Idx < VerticesColor.Num())
    {
        return VerticesColor[Idx];
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
        std::stringstream TokenStream(Line);
        TArray<std::string> Tokens;
        std::string Token;
        while (std::getline(TokenStream, Token, ' '))
        {
            Tokens.Add(Token);
        }
        if (Tokens.IsEmpty()) return;
        const std::string& Key = Tokens[0];
        if (Key == "mtllib") 
        {
            MaterialPath = "Resources/" + Tokens[1];
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
            VerticesColor.Add(Color);
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
            CurrentMaterial = FString{Tokens[1]};
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
            FaceMaterials.Add(CurrentMaterial); //faces 배열과 동일한 순서로 머티리얼 이름 기록
        }
    }
    File.close();
   
    ReadMaterialFile();
    CreateSubMesh();
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
            ObjMaterialInfo.map_Ka = Tokens[1];
        }
        else if (Key == TEXT("map_Kd")) {
            ObjMaterialInfo.map_Kd = Tokens[1];
        }
        else if (Key == TEXT("map_Ks")) {
            ObjMaterialInfo.map_Ks = Tokens[1];
        }
        else if (Key == TEXT("map_Ns")) {
            ObjMaterialInfo.map_Ns = Tokens[1];
        }
        else if (Key == TEXT("map_d")) {
            ObjMaterialInfo.map_d = Tokens[1];
        }
        else if (Key == TEXT("map_bump")) {
            ObjMaterialInfo.map_bump = Tokens[1];
        }
        else if (Key == TEXT("map_refl")) {
            ObjMaterialInfo.map_refl = Tokens[1];
        }
        RawData.MaterialList[CurrentMaterial] = ObjMaterialInfo;
    }

}

void ObjReader::CreateSubMesh()
{
    TMap<FString, TArray<uint32>> MaterialIndexMap;
    for (int i = 0; i < Faces.Num(); ++i) {
        const FString& MatName = FaceMaterials[i];
        TArray<uint32>* pIndices = MaterialIndexMap.Find(MatName);
        if (pIndices == nullptr)
        {
            MaterialIndexMap[MatName]= TArray<uint32>();
            pIndices = MaterialIndexMap.Find(MatName);
        }
        TArray<TArray<uint32>>& face = Faces[i];
        for (int32 j = 0; j < 3; ++j)
        {
            pIndices->Add(face[j][0]);
        }
    }
    int32 currentStartIndex = 0;
    for (auto it = MaterialIndexMap.begin(); it != MaterialIndexMap.end(); ++it)
    {
        FSubMesh Submesh;
        TArray<uint32>& indices = it->Value; // 해당 머티리얼의 인덱스 배열
        int32 count = indices.Num();
     
        Submesh.startIndex = currentStartIndex;
        Submesh.endIndex = currentStartIndex + count - 1;

        Submesh.materialName = it->Key; // subMesh에 머티리얼 이름 저장

        SubMeshes.Add(Submesh);
        currentStartIndex += count;
    }
}
