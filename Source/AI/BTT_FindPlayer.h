// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_FindPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERCONTROLLER_API UBTT_FindPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTT_FindPlayer(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI",meta=(AllowPrivateAccess=true))
	bool SearchRandom = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI",meta=(AllowPrivateAccess=true))
	float SearchRadius = 550.f;
};
