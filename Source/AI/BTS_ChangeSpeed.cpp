// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTS_ChangeSpeed.h"

#include "BaseAI.h"
#include "BaseAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTS_ChangeSpeed::UBTS_ChangeSpeed(const FObjectInitializer& ObjectInitializer)
{
	bNotifyBecomeRelevant = true;
	NodeName = "ChangeSpeed";
}

void UBTS_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	if (auto* const Cont = Cast<ABaseAIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const AI = Cast<ABaseAI>(Cont->GetPawn()))
		{
			AI->GetCharacterMovement()->MaxWalkSpeed = Speed;
		}
	}
}
