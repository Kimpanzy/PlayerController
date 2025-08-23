// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "BaseAIController.generated.h"

/**
 * 
 */
UCLASS()
class PLAYERCONTROLLER_API ABaseAIController : public AAIController
{
	GENERATED_BODY()
public:
	ABaseAIController();
protected:
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;
private:
	class UAISenseConfig_Sight* SightConfig;

	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetection(AActor* Actor, FAIStimulus const Stimilus);
};
