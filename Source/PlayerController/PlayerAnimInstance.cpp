// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerController/PlayerAnimInstance.h"
#include "PlayerController/PlayerPawn.h"
#include "Kismet/KismetMathLibrary.h"


void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	PlayerCharacter = Cast<APlayerPawn>(TryGetPawnOwner());
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Anim Instance Initialized"));	
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (PlayerCharacter)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(PlayerCharacter->GetMovementSpeed());
		bIsFalling = PlayerCharacter->GetIsFalling();
		CharacterState = PlayerCharacter->GetCharacterState();
		ActionState = PlayerCharacter->GetActionState();
	}
	
}
