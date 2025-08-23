// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_ChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERCONTROLLER_API UBTT_ChasePlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTT_ChasePlayer(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
