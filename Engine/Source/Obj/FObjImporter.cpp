#include "pch.h"
// FObjImporter.cpp
#include "FObjImporter.h"
#include "UStaticMesh.h"
#include <fstream>
#include <sstream>

#include "Engine.h"

REGISTER_CLASS(FObjImporter);
bool FObjImporter::LoadObjFile(const std::string& path, FObjInfo& OutInfo)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

    OutInfo.FileName = path;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            OutInfo.Positions.emplace_back(x, -y, z); // Blender to Unreal
        } else if (type == "vt") {
            float u, v;
            iss >> u >> v;
            OutInfo.UVs.emplace_back(u, 1.0f - v); // Flip V axis
        } else if (type == "vn") {
            float x, y, z;
            iss >> x >> y >> z;
            OutInfo.Normals.emplace_back(x, -y, z);
        } else if (type == "f") {
            std::string token;
            for (int i = 0; i < 3; ++i) {
                iss >> token;
                uint32_t vi, ti, ni;
                ParseFaceToken(token, vi, ti, ni);

                OutInfo.VertexIndices.push_back(vi - 1);
                OutInfo.UVIndices.push_back(ti - 1);
                OutInfo.NormalIndices.push_back(ni - 1);
            }
        }
    }

    return true;
}

void FObjImporter::ParseFaceToken(const std::string& token, uint32_t& vi, uint32_t& ti, uint32_t& ni)
{
    sscanf_s(token.c_str(), "%d/%d/%d", &vi, &ti, &ni);
}

FStaticMesh* FObjImporter::ConvertToStaticMesh(const FObjInfo& Info)
{
    FStaticMesh* Mesh = new FStaticMesh();
    Mesh->PathFileName = Info.FileName;

    for (size_t i = 0; i < Info.VertexIndices.size(); ++i) {
        FNormalVertex vertex;
        vertex.pos = Info.Positions[Info.VertexIndices[i]];
        vertex.tex = Info.UVs[Info.UVIndices[i]];
        vertex.normal = Info.Normals[Info.NormalIndices[i]];
        vertex.color = FVector4(1, 1, 1, 1);

        Mesh->Vertices.push_back(vertex);
        Mesh->Indices.push_back(static_cast<uint32_t>(i));
    }
    URenderer* Renderer = UEngine::Get().GetRenderer();
    Mesh->VertexBuffer = Renderer->CreateImmutableVertexBuffer(Mesh->Vertices.data(), Mesh->Vertices.size());
    Mesh->IndexBuffer  = Renderer->CreateIndexBuffer(Mesh->Indices.data(), Mesh->Indices.size());

    return Mesh;
}
