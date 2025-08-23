// Fill out your copyright notice in the Description page of Project Settings.


#include "Patrol/BTT_IncrementPathIndex.h"

#include "AI/BaseAI.h"
#include "AI/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_IncrementPathIndex::UBTT_IncrementPathIndex(const FObjectInitializer& ObjectInitializer)
{
	NodeName = "IncrementPathIndex";
}

EBTNodeResult::Type UBTT_IncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Cont = Cast<ABaseAIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const AI = Cast<ABaseAI>(Cont->GetPawn()))
		{
			if (auto* const Bc = OwnerComp.GetBlackboardComponent())
			{
				if (auto const NoOfPoints = AI->GetPatrolPath()->Num())
				{
					
					auto const MinIndex = 0;
					auto const MaxIndex = NoOfPoints - 1;
					auto Index = Bc->GetValueAsInt(GetSelectedBlackboardKey());

					if (bBiDirectional)
					{
						if (Index >= MaxIndex && Direction == EDirectionType::Forward)
						{
							Direction = EDirectionType::Reverse;
						}
						else if (Index == MinIndex && Direction == EDirectionType::Reverse)
						{
							Direction = EDirectionType::Forward;
						}
					}

					Bc->SetValueAsInt(GetSelectedBlackboardKey(),
						(Direction == EDirectionType::Forward ? ++Index : --Index) %NoOfPoints);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}
