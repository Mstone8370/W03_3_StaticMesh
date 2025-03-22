#pragma once
#include "Container/Array.h"
#include "Container/String.h"
#include "Rendering/RendererDefine.h"

class ObjReader
{
public:
    ObjReader();
    
    ObjReader(const FString& InFilePath);
    
    ~ObjReader();

    TArray<float> GetVertex(int32 Idx) const;
    
    TArray<float> GetNormal(int32 Idx) const;
    
    TArray<float> GetUV(int32 Idx) const;

    TArray<uint32> GetVertexIndices();

    TArray<TArray<TArray<uint32>>> GetFaces() const;

    uint32 GetFaceNum() const { return FObjRawData.VertexIndices.Num(); }

protected:
    FString FilePath;
    
    std::ifstream File;

    void SetFilePath(const FString& InFilePath);

    bool CheckFile(const FString& InFilePath) const;
    
    void ReadFile();

    void Clear();

    FObjInfo FObjRawData;
    TArray<FObjSubMesh> SubMeshes;
    FString CurrentMaterial;
    TArray<TArray<uint32>> Face;
};
