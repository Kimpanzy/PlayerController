// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTS_IsPlayerInMeleeRange.h"

#include "BaseAI.h"
#include "BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"


UBTS_IsPlayerInMeleeRange::UBTS_IsPlayerInMeleeRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = "IsPlayerInMeleeRange";
}

void UBTS_IsPlayerInMeleeRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* const Cont = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	auto* const AI = Cast<ABaseAI>(Cont->GetPawn());

	auto const Player = UGameplayStatics::GetPlayerPawn(GetWorld(),0);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(
		GetSelectedBlackboardKey(),
		AI->GetDistanceTo(Player) <= MeleeRange);
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}
