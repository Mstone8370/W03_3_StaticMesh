#include "pch.h" 
#include "BufferCache.h"

#include "ObjManager.h"
#include "Engine/Engine.h"
#include "GameFrameWork/StaticMesh.h"
#include "Primitive/PrimitiveVertices.h"
#include "Static/ObjManager.h"

FBufferCache::FBufferCache()
{
}

FBufferCache::~FBufferCache()
{
}

void FBufferCache::Init()
{

}

FVertexBufferInfo FBufferCache::GetVertexBufferInfo(EPrimitiveType Type)
{
    if (!VertexBufferCache.Contains(Type))
    {
        auto bufferInfo = CreateVertexBufferInfo(Type);
		VertexBufferCache.Add(Type, bufferInfo);
    }

    return VertexBufferCache[Type];
}

FIndexBufferInfo FBufferCache::GetIndexBufferInfo(EPrimitiveType Type)
{
    if (!IndexBufferCache.Contains(Type))
    {
		auto bufferInfo = CreateIndexBufferInfo(Type);
		IndexBufferCache.Add(Type, bufferInfo);
    }

	return IndexBufferCache[Type];
}

FStaticMeshBufferInfo FBufferCache::GetStaticMeshBufferInfo(FName InName)
{
    if (StaticMeshBufferCache.Contains(InName))
    {
        return StaticMeshBufferCache[InName];
    }
    return {};
}
bool FBufferCache::BuildStaticMesh(const FString& ObjFilePath)
{
    // FObjManager::LoadObjStaticMesh 함수가 FStaticMesh 객체를 생성하여 반환함
    UStaticMesh* StaticMesh = FObjManager::LoadObjStaticMesh(ObjFilePath);
    if (!StaticMesh) return false;

    // 파일 경로에서 파일 이름만 추출
    FString filePath = *ObjFilePath;

    FName Key = FObjImporter::GetNormalizedMeshKey(filePath);

    if (StaticMeshBufferCache.Contains(Key)) return true; 

    URenderer* Renderer = UEngine::Get().GetRenderer();

    FStaticMesh* StaticMeshAsset = StaticMesh->GetStaticMeshAsset();
    if (!StaticMeshAsset) return false;

    // FStaticMesh 객체 내부에 저장된 데이터를 사용
    uint32 VertexBufferByteWidth = StaticMeshAsset->Vertices.Num() * sizeof(FStaticMeshVertex);
    ID3D11Buffer* VertexBuffer = Renderer->CreateImmutableVertexBuffer(StaticMeshAsset->Vertices.GetData(), VertexBufferByteWidth);

    uint32 IndexBufferByteWidth = StaticMeshAsset->Indices.Num() * sizeof(uint32);
    ID3D11Buffer* IndexBuffer = Renderer->CreateIndexBuffer(StaticMeshAsset->Indices.GetData(), IndexBufferByteWidth);

    FVertexBufferInfo VertexInfo(VertexBuffer, StaticMeshAsset->Vertices.Num(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, nullptr);
    FIndexBufferInfo IndexInfo(IndexBuffer, StaticMeshAsset->Indices.Num());
    FVector Min = FVector(FLT_MAX,FLT_MAX, FLT_MAX);
    FVector Max = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    FObjImporter::CalculateBoundingBox(StaticMeshAsset->Vertices, Min, Max);
   
    VertexInfo.SetMax(Max);
    VertexInfo.SetMin(Min);

    FStaticMeshBufferInfo StaticMeshInfo(VertexInfo, IndexInfo);
    StaticMeshBufferCache.Add(Key, StaticMeshInfo);

    return true;
}


FVertexBufferInfo FBufferCache::CreateVertexBufferInfo(EPrimitiveType Type)
{
    ID3D11Buffer* VertexBuffer = nullptr;
    int VerticeSize = 0;
    D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    FVertexSimple* Vertices = nullptr;

    // !TODO : 모든 프리미티브들에 대해서 동일한 방법의 캐싱 사용
    switch (Type)
    {
    case EPrimitiveType::EPT_Line:
        VerticeSize = std::size(LineVertices);
        Vertices = LineVertices;
        Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case EPrimitiveType::EPT_Triangle:
        VerticeSize = std::size(TriangleVertices);
        Vertices = TriangleVertices;
        break;
    case EPrimitiveType::EPT_Cube:
        VerticeSize = std::size(CubeVertices);
        Vertices = CubeVertices;
        break;
    case EPrimitiveType::EPT_Sphere:
        VerticeSize = std::size(SphereVertices);
        Vertices = SphereVertices;
        break;
    case EPrimitiveType::EPT_Cylinder:
    {
		PrimitiveVertices.Add(EPrimitiveType::EPT_Cylinder, CreateCylinderVertices());
		VerticeSize = PrimitiveVertices[EPrimitiveType::EPT_Cylinder].Num();
        Vertices = PrimitiveVertices[EPrimitiveType::EPT_Cylinder].GetData();
        break;
    }
    case EPrimitiveType::EPT_Cone:
    {
        PrimitiveVertices.Add(EPrimitiveType::EPT_Cone, CreateConeVertices());
        VerticeSize = PrimitiveVertices[EPrimitiveType::EPT_Cone].Num();
        Vertices = PrimitiveVertices[EPrimitiveType::EPT_Cone].GetData();
        break;
    }
    }
    VertexBuffer = UEngine::Get().GetRenderer()->CreateImmutableVertexBuffer(Vertices, sizeof(FVertexSimple) * VerticeSize);

    return FVertexBufferInfo(VertexBuffer, VerticeSize, Topology, Vertices);
}

FIndexBufferInfo FBufferCache::CreateIndexBufferInfo(EPrimitiveType Type)
{
	ID3D11Buffer* IndexBuffer = nullptr;
	UINT* Indices = nullptr;
    int Size = 0;

    switch (Type)
    {

    case EPrimitiveType::EPT_Cube:
    {
        static std::vector<UINT> IndiceArr =
        {
            // 앞면
            0, 1, 2, 2, 1, 3,
            // 뒷면
            4, 6, 5, 5, 6, 7,
            // 왼쪽면
            0, 2, 4, 4, 2, 6,
            // 오른쪽면
            1, 5, 3, 3, 5, 7,
            // 윗면
            2, 3, 6, 6, 3, 7,
            // 아랫면
            0, 4, 1, 1, 4, 5
        };
        Indices = IndiceArr.data();
        Size = IndiceArr.size();
    }

    }

	IndexBuffer = Indices == nullptr ? nullptr : UEngine::Get().GetRenderer()->CreateIndexBuffer(Indices, sizeof(UINT) * Size);
    return FIndexBufferInfo(IndexBuffer, Size);
}

TArray<UINT> FBufferCache::CreateDefaultIndices(int Size)
{
    TArray<UINT> Indices;
	for (int i = 0; i < Size; ++i)
	{
		Indices.Add(i);
	}

    return Indices;
}


TArray<FVertexSimple> FBufferCache::CreateConeVertices()
{
    TArray<FVertexSimple> vertices;

    int segments = 36;
    float radius = 1.f;
    float height = 1.f;

	// Cone bottom
    for (int i = 0; i < segments; ++i)
    {
        float angle = 2.0f * PI * i / segments;
        float nextAngle = 2.0f * PI * (i + 1) / segments;

        float x1 = radius * cos(angle);
        float y1 = radius * sin(angle);
        float x2 = radius * cos(nextAngle);
        float y2 = radius * sin(nextAngle);

        // Bottom Triangles(CCW)
        vertices.Add({ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });

        // Side Triangles(CW)
        vertices.Add({ x1, y1, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ 0.0f, 0.0f, height, 0.0f, 1.0f, 0.0f, 1.0f });
    }

    return vertices;
}

TArray<FVertexSimple> FBufferCache::CreateCylinderVertices()
{
    TArray<FVertexSimple> vertices;
        
    int segments = 36;
    float radius = 1.f;
    float height = 1.f;


	// Cylinder bottom top
    for (int i = 0; i < segments; ++i)
    {
        float angle = 2.0f * PI * i / segments;
        float nextAngle = 2.0f * PI * (i + 1) / segments;

        float x1 = radius * cos(angle);
        float y1 = radius * sin(angle);
        float x2 = radius * cos(nextAngle);
        float y2 = radius * sin(nextAngle);

        // Bottom Triangles(CCW)
        vertices.Add({ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
        vertices.Add({ x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });

        // Top Triangles(CCW)
        vertices.Add({ 0.0f, 0.0f, height, 0.0f, 1.0f, 0.0f, 1.0f });
        vertices.Add({ x1, y1, height, 0.0f, 1.0f, 0.0f, 1.0f });
        vertices.Add({ x2, y2, height, 0.0f, 1.0f, 0.0f, 1.0f });

        // Side(Outer) Triangles(CW)
        vertices.Add({ x1, y1, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x1, y1, height, 0.0f, 0.0f, 1.0f, 1.0f });

        vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x2, y2, height, 0.0f, 0.0f, 1.0f, 1.0f });
        vertices.Add({ x1, y1, height, 0.0f, 0.0f, 1.0f, 1.0f });
    }

    return vertices;
}
