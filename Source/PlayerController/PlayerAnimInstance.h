// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerController/CharacterStates.h"
#include "PlayerAnimInstance.generated.h"

class APlayerPawn;
UCLASS()
class PLAYERCONTROLLER_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	APlayerPawn* PlayerCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement");
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bHoldingLedge = false;
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
	ECharacterState CharacterState;
	UPROPERTY(BlueprintReadOnly, Category = "Movement | Action State")
	EActionState ActionState;
	
};
