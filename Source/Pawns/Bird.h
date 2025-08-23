#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
UCLASS()
class PLAYERCONTROLLER_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Capsule",meta=(AllowPrivateAccess))
	UCapsuleComponent* BirdCapsule;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category="Mesh",meta=(AllowPrivateAccess))
	USkeletalMeshComponent* BirdMesh;
};
