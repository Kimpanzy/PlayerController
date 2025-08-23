// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_ChasePlayer.h"

#include "BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTT_ChasePlayer::UBTT_ChasePlayer(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "ChasePlayer";
}

EBTNodeResult::Type UBTT_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Cont = Cast<ABaseAIController>(OwnerComp.GetAIOwner()))
	{
		auto const PlayerLoc = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cont,PlayerLoc + 30.f);

		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
