// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AttackAnimNotifyState.h"

#include "AI/BaseParentAI.h"
#include "PlayerController/PlayerPawn.h"

void UAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABaseParentAI* const AI = Cast<ABaseParentAI>(MeshComp->GetOwner()))
		{
			AI->AttackStart();
		}
		if (APlayerPawn* const Player = Cast<APlayerPawn>(MeshComp->GetOwner()))
		{
			Player->AttackStart();
		}
	}
}

void UAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABaseParentAI* const AI = Cast<ABaseParentAI>(MeshComp->GetOwner()))
		{
			AI->AttackEnd();
		}
		if (APlayerPawn* const Player = Cast<APlayerPawn>(MeshComp->GetOwner()))
		{
			Player->AttackEnd();
		}
	}
}
