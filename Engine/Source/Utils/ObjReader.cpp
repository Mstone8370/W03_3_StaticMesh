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
    if (0 <= Idx && Idx < Vertices.Num())
    {
        return Vertices[Idx];
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
    if (0 <= Idx && Idx < Normals.Num())
    {
        return Normals[Idx];
    }
    return {};
}

TArray<float> ObjReader::GetUV(int32 Idx)
{
    if (0 <= Idx && Idx < UVs.Num())
    {
        return UVs[Idx];
    }
    return {};
}

TArray<uint32> ObjReader::GetVertexIndices()
{
    TArray<uint32> Indices(Vertices.Num() * 3);
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
    for (TArray<float>& Vertex : Vertices)
    {
        Vertex.Empty();
    }
    Vertices.Empty();

    for (TArray<float>& Normal : Normals)
    {
        Normal.Empty();
    }
    Normals.Empty();

    for (TArray<float>& UV : UVs)
    {
        UV.Empty();
    }
    UVs.Empty();

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
            Vertices.Add(Vertex);
        }
        else if (Key == "vn")
        {
            TArray<float> Normal(3);
            Normal[0] = std::stof(Tokens[1]);  // Normal X
            Normal[1] = -std::stof(Tokens[2]); // Normal Y
            Normal[2] = std::stof(Tokens[3]);  // Normal Z
            Normals.Add(Normal);
        }
        else if (Key == "vt")
        {
            TArray<float> UV(2);
            UV[0] = std::stof(Tokens[1]);       // U
            UV[1] = 1.f - std::stof(Tokens[2]); // V; Obj 파일은 오른손 좌표계 기준이므로, 왼손 좌표계의 UV맵 좌표로 변경
            UVs.Add(UV);
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
        MaterialInfo[CurrentMaterial] = ObjMaterialInfo;
    }

}
std::wstring ConvertToWide(const char* str)
{
    // 변환에 필요한 버퍼 길이 계산
    int len = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
    if (len == 0)
        return L"";

    // 변환된 문자열을 저장할 wstring 생성
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, str, -1, &wstr[0], len);

    // 마지막 null 문자 제거 (필요 시)
    if (!wstr.empty() && wstr.back() == L'\0')
        wstr.pop_back();

    return wstr;
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

    for (int i = 0; i < SubMeshes.Num(); ++i)
    {
        FSubMesh& sm = SubMeshes[i];
        FObjMaterialInfo material = MaterialInfo[sm.materialName];
        std::wstring output = L"\nSubMesh Material:" + ConvertToWide(sm.materialName.c_char()) + L":\n";
        output += TEXT(" StartIndices ") + std::to_wstring(sm.startIndex) + L":\n";
        output += TEXT(" EndIndices ") + std::to_wstring(sm.endIndex) + L":\n";
        output += TEXT("Material Info:\n");
        output += TEXT("  Ns: ") + std::to_wstring(material.Ns) + L"\n";
        output += TEXT("  Ka: (") + std::to_wstring(material.Ka.X) + L", " + std::to_wstring(material.Ka.Y) + L", " + std::to_wstring(material.Ka.Z) + L")\n";
        output += TEXT("  Ks: (") + std::to_wstring(material.Ks.X) + L", " + std::to_wstring(material.Ks.Y) + L", " + std::to_wstring(material.Ks.Z) + L")\n";
        output += TEXT("  Ke: (") + std::to_wstring(material.Ke.X) + L", " + std::to_wstring(material.Ke.Y) + L", " + std::to_wstring(material.Ke.Z) + L")\n";
        output += TEXT("  Ni: " )+ std::to_wstring(material.Ni) + L"\n";
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
}
