#pragma once
#include "Container/Array.h"
#include "Container/Map.h"
#include "Container/String.h"

class UStaticMesh;
struct FStaticMeshVertex;

struct FStaticMesh;
struct FObjInfo;
struct FObjMaterialInfo;
struct FObjImporter;

class FObjManager
{
public:
    static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);

    static UStaticMesh* LoadObjStaticMesh(const FString& PathFileName);

private:
    static TMap<FString, FStaticMesh*> ObjStaticMeshMap;
};


// Cooked Data
struct FStaticMesh
{
    std::string PathFileName;

    TArray<FStaticMeshVertex> Vertices;
    TArray<uint32> Indices;
};

// Raw Data
struct FObjInfo
{
    // Vertex List
    // UV List
    // Normal List
    // Vertex Index List
    // UV Index List
    // Normal Index List
    // Material List
    // Texture List
};

struct FObjMaterialInfo
{
    // Diffuse Scalar
    // Diffuse Texture
};

struct FObjImporter
{
    // Obj Parsing (*.obj to FObjInfo)
    // Material Parsing (*.obj to MaterialInfo)
    // Convert the Raw data to Cooked data (FStaticMesh)
};