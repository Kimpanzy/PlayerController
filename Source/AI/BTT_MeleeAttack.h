// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAI.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERCONTROLLER_API UBTT_MeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTT_MeleeAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	bool MontageHasFinished(ABaseAI* const AI);
};
