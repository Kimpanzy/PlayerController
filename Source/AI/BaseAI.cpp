#include "AI/BaseAI.h"

ABaseAI::ABaseAI()
{
	PrimaryActorTick.bCanEverTick = true;
	
}

UBehaviorTree* ABaseAI::GetBehaviorTree() const
{
	return Tree;
}

APatrolPath* ABaseAI::GetPatrolPath() const
{
	return PatrolPath;
}

UAnimMontage* ABaseAI::GetAnimMontage() const
{
	return AttackMontage;
}

void ABaseAI::PlayStrangleMontage()
{
	PlayAnimMontage(StrangleMontage);
	bDead = true;
}

int ABaseAI::MeleeAttack_Implementation()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
	}
	return 0;
}

bool ABaseAI::GetIsDead()
{
	return bDead;
}

void ABaseAI::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

