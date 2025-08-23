// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindPathPoint.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERCONTROLLER_API UBTT_FindPathPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTT_FindPathPoint(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
private:
	UPROPERTY(EditAnywhere,Category="Blackboard",meta=(AllowPrivateAccess=true))
	FBlackboardKeySelector PatrolPathVectorKey;
};
