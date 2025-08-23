// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTT_MeleeAttack.h"

#include "BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_MeleeAttack::UBTT_MeleeAttack()
{
	NodeName = "MeleeAttack";
}

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto const OutOfRange = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	if (OutOfRange)
	{
		return EBTNodeResult::Succeeded;
	}

	auto* const Cont = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	auto* const AI = Cast<ABaseAI>(Cont->GetPawn());

	if (auto* const iCombat = Cast<ICombatInterface>(AI))
	{
		if (MontageHasFinished(AI))
		{
			iCombat->Execute_MeleeAttack(AI);
		}
	}
	return EBTNodeResult::Succeeded;
}

bool UBTT_MeleeAttack::MontageHasFinished(ABaseAI* const AI)
{
	return AI->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(AI->GetAnimMontage());
}
