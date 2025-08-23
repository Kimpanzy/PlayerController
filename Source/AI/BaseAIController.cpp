// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BaseAIController.h"

#include "BaseAI.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PlayerController/PlayerPawn.h"


ABaseAIController::ABaseAIController()
{
	SetupPerceptionSystem();
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ABaseAI* AI = Cast<ABaseAI>(InPawn))
	{
		if (UBehaviorTree* const Tree = AI->GetBehaviorTree())
		{
			UBlackboardComponent* BlackboardComponent = nullptr;
			if (UseBlackboard(Tree->BlackboardAsset, BlackboardComponent))
			{
				Blackboard = BlackboardComponent;
				RunBehaviorTree(Tree);
			}
		}
	}
}

void ABaseAIController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	if (SightConfig)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(
			TEXT("Perception Component")));
		SightConfig->SightRadius = 500.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this,&ABaseAIController::OnTargetDetection);
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
}

void ABaseAIController::OnTargetDetection(AActor* Actor, FAIStimulus const Stimilus)
{
	if (auto* const Player = Cast<APlayerPawn>(Actor))
	{
		GetBlackboardComponent()->SetValueAsBool("CanSeePlayer",Stimilus.WasSuccessfullySensed());
	}
}
