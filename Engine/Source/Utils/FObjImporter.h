#pragma once

#include "pch.h" 
#include "Container/Array.h"
#include "Container/Map.h"
#include "Container/String.h"
#include "Core/Math/Vector.h"
#include "Rendering/RendererDefine.h"

struct FObjImporter
{
    FObjInfo rawData;

    //// *.obj -> Raw(FObjInfo)
    //static bool ParseObj(const FString& PathFileName, FObjInfo& OutInfo);

    //// Raw(FObjInfo) -> Cooked(FStaticMesh)
    //static bool ConvertToStaticMesh(const FObjInfo& InInfo, FStaticMesh& OutMesh);
    
    static bool LoadStaticMesh(const FString& Path, FStaticMesh& OutMesh)
    {
        //FObjInfo Raw;
        //if (!ParseObj(Path, Raw)) return false;
        //return ConvertToStaticMesh(Raw, OutMesh);
    }
};