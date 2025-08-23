#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseParentAI.generated.h"

UCLASS()
class PLAYERCONTROLLER_API ABaseParentAI : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseParentAI();
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	float GetMaxHealth()const;
	float GetHealth()const;
	void SetHealth(float const NewHealth);

	void AttackStart() const;
	void AttackEnd() const;
protected:
	virtual void BeginPlay() override;
private:
	
	UPROPERTY()
	class UWidgetComponent* WidgetComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Health",meta=(AllowPrivateAccess="true"))
	float MaxHealth{100.f};
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Health",meta=(AllowPrivateAccess="true"))
	float Health;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Collision",meta=(AllowPrivateAccess="true"))
	class UBoxComponent* RightFistBox;
	UPROPERTY()
	mutable bool bHasHitPlayer = false;
	UFUNCTION()
	void OnAttackOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int const OtherBodyIndex,
		bool const FromSweep,
		FHitResult const &SweepResult);
	UFUNCTION()
	void OnAttackOverlapEnd(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int const OtherBodyIndex);

};
