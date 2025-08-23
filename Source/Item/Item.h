#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped,
};

UCLASS()
class PLAYERCONTROLLER_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintPure)
	float TransformSin() const;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Sine Parameters")
	float Amplitude = 0.25;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Sine Parameters")
	float TimeConstant = 5.f;
	UPROPERTY(EditAnywhere)
	float RunningTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = "Name", meta = (AllowPrivateAccess = "true"))
	FName ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,Category = "SphereCollider", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Sphere;

	EItemState ItemState = EItemState::EIS_Hovering;
private:
};
