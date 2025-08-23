// Fill out your copyright notice in the Description page of Project Settings.


#include "Patrol/BTT_FindPathPoint.h"

#include "AI/BaseAI.h"
#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_FindPathPoint::UBTT_FindPathPoint(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "FindPathPoint";
}

EBTNodeResult::Type UBTT_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Cont = Cast<ABaseAIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const Bc = OwnerComp.GetBlackboardComponent())
		{
			auto const Index = Bc->GetValueAsInt(GetSelectedBlackboardKey());

			if (auto* const AI = Cast<ABaseAI>(Cont->GetPawn()))
			{
				auto const Point = AI->GetPatrolPath()->GetPatrolPoint(Index);

				auto const GlobalPoint = AI->GetPatrolPath()->GetActorTransform().TransformPosition(Point);

				Bc->SetValueAsVector(PatrolPathVectorKey.SelectedKeyName, GlobalPoint);

				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
