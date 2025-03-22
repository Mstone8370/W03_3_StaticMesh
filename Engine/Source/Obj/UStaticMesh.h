// UStaticMesh.h
#pragma once

#include <string>
#include <vector>

#include "Object.h"
#include "Math/Vector.h"

struct FNormalVertex {
    
    FVector pos;
    FVector normal;
    FVector4 color;
    FVector2D tex;
};

struct FStaticMesh {
    std::string PathFileName;
    std::vector<FNormalVertex> Vertices;
    std::vector<uint32_t> Indices;
    ID3D11Buffer* VertexBuffer = nullptr;  // 추가
    ID3D11Buffer* IndexBuffer = nullptr;   // 추가
    ~FStaticMesh()
    {
        if (VertexBuffer) { VertexBuffer->Release(); }
        if (IndexBuffer) { IndexBuffer->Release(); }
    }
};

class UStaticMesh : public UObject {
private:
    FStaticMesh* StaticMeshAsset;

public:
    UStaticMesh();
    const std::string& GetAssetPathFileName() const;
    void SetStaticMeshAsset(FStaticMesh* InStaticMesh);
    FStaticMesh* GetStaticMeshAsset();
};
