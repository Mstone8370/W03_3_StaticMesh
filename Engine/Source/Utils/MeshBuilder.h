#pragma once
#include "Container/Array.h"
#include "Container/String.h"
#include "Math/Vector.h"
#include "Rendering/RendererDefine.h"
#include "Utils/ObjReader.h"

class MeshBuilder
{
public:
    MeshBuilder();

    ~MeshBuilder();

    // *.obj -> Raw(FObjInfo)
    static bool ParseObj(const FString& PathFileName, FObjInfo& OutInfo);

    // Raw(FObjInfo) -> Cooked(FStaticMesh)
    static bool ConvertToStaticMesh(const FObjInfo& InInfo, FStaticMesh& OutMesh);

    bool BuildMeshFromObj(const ObjReader& Reader);

    uint32 GetVertexNum() const { return StaticMesh.Vertices.Num(); }
    uint32 GetIndexNum() const { return StaticMesh.Indices.Num(); }

    TArray<FNormalVertex> GetVertices() { return StaticMesh.Vertices; }
    TArray<uint32> GetIndices() { return StaticMesh.Indices; }

private:
    void MakeVertex(const TArray<float>& Vertex, const TArray<float>& Normal, const TArray<float>& UV, FNormalVertex& OutVertex);

    void CalculateTangent(const FNormalVertex& Vertex0, const FNormalVertex& Vertex1, const FNormalVertex& Vertex2, FVector& OutTangent);

    uint32 VerticesNum;
    uint32 IndicesNum;

    FStaticMesh StaticMesh;
};
