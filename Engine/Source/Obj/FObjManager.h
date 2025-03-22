// FObjManager.h
#pragma once

#include <string>
#include <map>
#include "FObjImporter.h"

class UStaticMesh;
class FStaticMesh;

class FObjManager {
private:
    static std::map<std::string, FStaticMesh*> ObjStaticMeshMap;

public:
    static FStaticMesh* LoadObjStaticMeshAsset(const std::string& PathFileName);
    static UStaticMesh* LoadObjStaticMesh(const std::string& PathFileName);
};