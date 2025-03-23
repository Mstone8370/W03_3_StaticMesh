#pragma once
#include "ObjManager.h"

struct FStaticMesh;
struct FObjMaterialInfo;
struct FSubMesh;
//Raw Data
struct FObjInfo;

class ObjReader
{
public:
    ObjReader();
    
    ObjReader(const FString& InFilePath);
    
    ~ObjReader();

    TArray<float> GetVertex(int32 Idx);

    TArray<float> GetVertexColor(int32 Idx);
    
    TArray<float> GetNormal(int32 Idx);
     
    TArray<float> GetUV(int32 Idx);
     
    TArray<TArray<TArray<uint32>>> GetFaces() { return Faces; }

    TArray<uint32> GetVertexIndices();

    TArray<uint32> GetUVIndices();

    TArray<uint32> GetNormalIndices();

    FObjInfo GetRawData();
    
    TMap<FName, FObjMaterialInfo> GetMaterialList() { return RawData.MaterialList; }

    TArray<FSubMesh> GetSubMeshes() { return SubMeshes; }

    TArray<FName> GetMaterialsName() { return MaterialsName; }

    uint32 GetFaceNum() const { return Faces.Num(); }

protected:
    FString FilePath;
    
    std::ifstream File;

    void SetFilePath(const FString& InFilePath);

    bool CheckFile(const FString& InFilePath) const;
    
    void ReadFile();

    void ReadMaterialFile();

    void CreateSubMesh();

    void Clear();

    TArray<TArray<float>> VerticesColor;
    FObjInfo RawData;
    TArray<TArray<TArray<uint32>>> Faces;

    FString MaterialPath;
    TArray<FSubMesh> SubMeshes;
    TArray<FName> FaceMaterials;
    TArray<FName> MaterialsName;
};
