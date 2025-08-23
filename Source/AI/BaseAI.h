// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseParentAI.h"
#include "CombatInterface.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Patrol/PatrolPath.h"
#include "BaseAI.generated.h"

UCLASS()
class PLAYERCONTROLLER_API ABaseAI : public ABaseParentAI, public ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	ABaseAI();

	UBehaviorTree* GetBehaviorTree() const;
	APatrolPath* GetPatrolPath() const;
	UAnimMontage* GetAnimMontage() const;
	void PlayStrangleMontage();
	int MeleeAttack_Implementation() override;
	UFUNCTION(BlueprintCallable)
	bool GetIsDead();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess = "true"))
	bool bDead = false;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;
private:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess = "true"))
	APatrolPath* PatrolPath;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	UAnimMontage* StrangleMontage;
	
	
};
