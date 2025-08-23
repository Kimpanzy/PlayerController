// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_FindPlayer.h"

#include "BaseAI.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTT_FindPlayer::UBTT_FindPlayer(FObjectInitializer const& ObjectInitializer)
{
	NodeName = "FindPlayer";
}

EBTNodeResult::Type UBTT_FindPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Player = UGameplayStatics::GetPlayerPawn(GetWorld(),0))
	{
		auto const PlayerLoc = Player->GetActorLocation();
		if (SearchRandom)
		{
			FNavLocation Loc;

			if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				if (NavSys->ProjectPointToNavigation(PlayerLoc,Loc,FVector(SearchRadius,SearchRadius,200.f)))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(),Loc.Location);
					return EBTNodeResult::Succeeded;
				}
					return EBTNodeResult::Failed;
			}
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(),PlayerLoc);
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}
