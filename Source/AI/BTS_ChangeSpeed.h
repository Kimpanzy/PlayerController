// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTS_ChangeSpeed.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERCONTROLLER_API UBTS_ChangeSpeed : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTS_ChangeSpeed(const FObjectInitializer& ObjectInitializer);
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Behavior",meta=(AllowPrivateAccess=true))
	float Speed = 600.f;
};
