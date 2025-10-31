#include "PCG/MeshCreator.h"
#include "ProceduralMeshComponent.h"
#include "PhysicsProxy/GeometryCollectionPhysicsProxy.h"


AMeshCreator::AMeshCreator()
{
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>("ProceduralMesh");
	RootComponent = ProceduralMesh;
}


void AMeshCreator::BeginPlay()
{
	Super::BeginPlay();
	CreateTriangle();
}

void AMeshCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeshCreator::CreateTriangle()
{
	TArray<FVector> Vertices;
	Vertices.Add(FVector(0, 0, 0));
	Vertices.Add(FVector(0, 100, 0));
	Vertices.Add(FVector(0, 0, 100));

	TArray<int32> Triangles = {0,1,2};
	
	TArray<FVector> Normals;
	Normals.Add(FVector(1,0,0));
	Normals.Add(FVector(1,0,0));
	Normals.Add(FVector(1,0,0));

	TArray<FVector2D> UV0 = { FVector2D(0,0), FVector2D(1,0), FVector2D(0,1) };

	TArray<FProcMeshTangent> Tangents;
	Tangents.Add(FProcMeshTangent(0,1,0));
	Tangents.Add(FProcMeshTangent(0,1,0));
	Tangents.Add(FProcMeshTangent(0,1,0));
	
	TArray<FColor> VertexColors = { FColor::White, FColor::White, FColor::White };

	ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
}
