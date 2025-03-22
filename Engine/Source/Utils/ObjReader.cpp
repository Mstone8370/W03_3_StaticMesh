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

TArray<float> ObjReader::GetVertex(int32 Idx) const 
{
    if (0 <= Idx && Idx < FObjRawData.Vertices.Num())
    {
        return FObjRawData.Vertices[Idx];
    }
    return {};
}

TArray<float> ObjReader::GetNormal(int32 Idx) const
{
    if (0 <= Idx && Idx < FObjRawData.Normals.Num())
    {
        return FObjRawData.Normals[Idx];
    }
    return {};
}

TArray<float> ObjReader::GetUV(int32 Idx) const
{
    if (0 <= Idx && Idx < FObjRawData.UVs.Num())
    {
        return FObjRawData.UVs[Idx];
    }
    return {};
}

TArray<uint32> ObjReader::GetVertexIndices()
{
    TArray<uint32> Indices(FObjRawData.Vertices.Num() * 3);
    for (const TArray<uint32>& FaceIndices : FObjRawData.VertexIndices)
    {
        for (uint32 index : FaceIndices)
        {
            Indices.Add(index);
        }
    }
    return Indices;
}

TArray<TArray<TArray<uint32>>> ObjReader::GetFaces() const
{
    TArray<TArray<TArray<uint32>>> Faces;

    uint32 FaceCount = FObjRawData.VertexIndices.Num();
    for (uint32 i = 0; i < FaceCount; ++i)
    {
        TArray<TArray<uint32>> Face;
        Face.Add(FObjRawData.VertexIndices[i]);
        Face.Add(FObjRawData.UVIndices[i]);
        Face.Add(FObjRawData.NormalIndices[i]);
        Faces.Add(Face);
    }
    return Faces;
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
    for (TArray<float>& Vertex : FObjRawData.Vertices)
    {
        Vertex.Empty();
    }
    FObjRawData.Vertices.Empty();

    for (TArray<float>& Normal : FObjRawData.Normals)
    {
        Normal.Empty();
    }
    FObjRawData.Normals.Empty();

    for (TArray<float>& UV : FObjRawData.UVs)
    {
        UV.Empty();
    }
    FObjRawData.UVs.Empty();

    for (TArray<uint32>& VertexIndex : FObjRawData.VertexIndices)
    {
        VertexIndex.Empty();
    }
    FObjRawData.VertexIndices.Empty();

    for (TArray<uint32>& UVIndex : FObjRawData.UVIndices)
    {
        UVIndex.Empty();
    }
    FObjRawData.UVIndices.Empty();

    for (TArray<uint32>& NormalIndex : FObjRawData.NormalIndices)
    {
        NormalIndex.Empty();
    }
    FObjRawData.NormalIndices.Empty();
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
    while (std::getline(File, Line))
    {
        std::stringstream TokenStream(Line);
        TArray<std::string> Tokens;
        std::string Token;
        while (std::getline(TokenStream, Token, ' '))
        {
            Tokens.Add(Token);
        }
        const std::string& Key = Tokens[0];

        if (Key == "v")
        {
            TArray<float> Vertex(3);
            Vertex[0] = std::stof(Tokens[1]);  // Location X
            Vertex[1] = -std::stof(Tokens[2]); // Location Y
            Vertex[2] = std::stof(Tokens[3]);  // Location Z
            FObjRawData.Vertices.Add(Vertex);
        }
        else if (Key == "vn")
        {
            TArray<float> Normal(3);
            Normal[0] = std::stof(Tokens[1]);  // Normal X
            Normal[1] = -std::stof(Tokens[2]); // Normal Y
            Normal[2] = std::stof(Tokens[3]);  // Normal Z
            FObjRawData.Normals.Add(Normal);
        }
        else if (Key == "vt")
        {
            TArray<float> UV(2);
            UV[0] = std::stof(Tokens[1]);       // U
            UV[1] = 1.f - std::stof(Tokens[2]); // V; Obj 파일은 오른손 좌표계 기준이므로, 왼손 좌표계의 UV맵 좌표로 변경
            FObjRawData.UVs.Add(UV);
        }
        else if (Key == "usemtl")
        {
            if (!CurrentMaterial.IsEmpty())
            {
                SubMeshes.Last().EndIdx = FObjRawData.VertexIndices.Num();
            }

            CurrentMaterial = Tokens[1];
        }
        else if (Key == "f")
        {
            for (int i = 0; i < 3; ++i)
            {
                TArray<uint32> FaceVertexIndices(3);
                TArray<uint32> FaceUVIndices(3);
                TArray<uint32> FaceNormalIndices(3);

                std::stringstream ss(Tokens[i + 1]);
                std::string Val;
                int Cnt = 0;
                while (std::getline(ss, Val, '/'))
                {
                    uint32 Index = std::stoi(Val) - 1; // 인덱스는 1부터 시작하므로, 1 감소
                    switch (Cnt)
                    {
                        case 0:
                            FaceVertexIndices[i] = Index;
                            break;
                        case 1:
                            FaceUVIndices[i] = Index;
                            break;
                        case 2:
                            FaceNormalIndices[i] = Index;
                            break;
                        default:
                            break;
                    }
                    ++Cnt;
                }
                FObjRawData.VertexIndices.Add(FaceVertexIndices);
                FObjRawData.VertexIndices.Add(FaceUVIndices);
                FObjRawData.VertexIndices.Add(FaceNormalIndices);
            }
        }
    }
    File.close();    
}
