#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshCreator.generated.h"

class UProceduralMeshComponent;
UCLASS()
class PLAYERCONTROLLER_API AMeshCreator : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeshCreator();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UProceduralMeshComponent* ProceduralMesh;
	UFUNCTION()
	void CreateTriangle();
protected:
	virtual void BeginPlay() override;
};
