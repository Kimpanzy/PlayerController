// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_FindRandomLocation.h"

#include "BaseAIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindRandomLocation::UBTT_FindRandomLocation(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "FindRandomLocation";
}

EBTNodeResult::Type UBTT_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Cont = Cast<ABaseAIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const AI = Cont->GetPawn())
		{
			auto const Origin = AI->GetActorLocation();

			if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation Loc;
				if (NavSys->GetRandomReachablePointInRadius(Origin,SearchRadius,Loc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(),Loc.Location);
				}

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
