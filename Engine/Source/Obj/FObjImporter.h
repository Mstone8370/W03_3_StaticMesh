// FObjImporter.h
#pragma once

#include <string>
#include <vector>
#include "Math/Vector.h"

struct FObjInfo {
    std::vector<FVector> Positions;
    std::vector<FVector2D> UVs;
    std::vector<FVector> Normals;

    std::vector<uint32_t> VertexIndices;
    std::vector<uint32_t> UVIndices;
    std::vector<uint32_t> NormalIndices;

    std::string FileName;
};

class FStaticMesh;

class FObjImporter {
public:
    static bool LoadObjFile(const std::string& path, FObjInfo& OutInfo);
    static FStaticMesh* ConvertToStaticMesh(const FObjInfo& Info);

private:
    static void ParseFaceToken(const std::string& token, uint32_t& vi, uint32_t& ti, uint32_t& ni);
};
