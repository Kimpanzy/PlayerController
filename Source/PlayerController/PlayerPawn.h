// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerController/CharacterStates.h"
#include "PlayerPawn.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UHealthBar;
class UCameraComponent;
class USpringArmComponent;
class UCapsuleComponent;
class UInputAction;
class UBoxComponent;
class USphereComponent;
class AItem;
class UAnimMontage;
class AWeapon;
class UMotionWarpingComponent;


DECLARE_MULTICAST_DELEGATE(FSpinDelegate);
UCLASS()
class PLAYERCONTROLLER_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();
	UPROPERTY(EditAnywhere)
	UCameraComponent* PlayerCamera;
	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* CapsuleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* VisualMesh;
	FRotator MeshOffset = (FRotator(0.f, -90.f, 0.f));
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	bool bIsAttacking;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Animation")
	FVector CachedAcceleration;
	FSpinDelegate Spinner;
	
	virtual void Tick(float DeltaTime) override;
	void RotateMeshToMovement(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	FVector GetMovementSpeed() const;
	void Move(FVector Movement);
	float GetMaxHealth() const;
	float GetHealth() const;
	FORCEINLINE void SetOverlappingItem(AItem* Item){OverlappingItem = Item;}
	FORCEINLINE ECharacterState GetCharacterState() const {return CharacterState;}
	FORCEINLINE EActionState GetActionState() const {return ActionState;}
	void SetHealth(float const NewHealth);
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMeshComponent()const;
	bool GetIsFalling() const;
	void AttackStart();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	/**
	 *  Ledge grabbing
	 **/
	UFUNCTION()
	void HeightTrace();
	UFUNCTION()
	void ForwardTrace();
	UFUNCTION()
	void PlayClimbingMontage();
	UFUNCTION()
	void RotateHangPos(float DeltaTime);
	UFUNCTION()
	void InputDropDown();
	UFUNCTION()
	void InputClimbUp();
	UFUNCTION(BlueprintCallable)
	void FinishClimb();
	UFUNCTION()
	void MoveCapsuleWithRoot();
	UFUNCTION()
	void MoveForwardAnim(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	void SetMovingForwardBool();
	UPROPERTY()
	FVector WallNormal;
	UPROPERTY()
	FVector WallLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage", meta=(AllowPrivateAccess = "true"))
	UAnimMontage* ClimbUpMontage;
	UPROPERTY()
	FVector TargetHangLocation;
	UPROPERTY()
	FRotator TargetHangRotation;
	UPROPERTY()
	bool bAnimDrivenMoveForward = false;
	bool bIsInterpolatingToHang = false;
	bool bIsClimbingMontage = false;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	FVector CurrentInput;
	UPROPERTY()
	FVector2D CameraInput;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Collision")
	UBoxComponent* FistBoxComponent;
	/**
	 * Movement / Physics
	 */
	FVector Direction = FVector(0.0f);
	FVector Velocity;
	UPROPERTY(EditAnywhere, Category = "TraceChannel")
	TEnumAsByte<ECollisionChannel> LedgeTrace = ECC_GameTraceChannel4;
	UPROPERTY(EditAnywhere, Category = "TraceChannel")
	TEnumAsByte<ECollisionChannel> EnemyTrace = ECC_GameTraceChannel5;
	UPROPERTY(EditAnywhere, Category = "Movement")
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double ColliderMargin = 1.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double JumpHeight = 500.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double JumpGravityMultiplier = 1.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double FallGravityMultiplier = 2.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double GravityScale;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double GroundCheckDistance = 10.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double Acceleration = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double Deceleration = 600.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double TerminalVelocity = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double AirResistance = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double StaticFrictionCoefficient = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Movement")
	double KineticFrictionCoefficient = 0.4f;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed = 600;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 600;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RootMotion")
	FVector PreviousRootMotionLocation = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UMotionWarpingComponent* MotionWarping;
	/**
	 * Inputs
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputMappingContext* PlayerInputContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* MovementAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* SpinAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* DashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* DropAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* AssassinateAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Enhanced Input")
	UInputAction* ClimbUpAction;
	/**
	 * Callbacks for inputs
	 */
	void Spin();
	void InputAttack();
	void InputInteract();
	void JumpInput();
	void InputSprint();
	void InputSprintStop();
	void InputLook(const FInputActionValue& Value);
	void InputMove(const FInputActionValue& Value);
	/**
	 * Play equip + attack montage functions
	 */
	void PlayAttackMontage() const;
	void PlayEquipMontage(FName SectionName) const;
	bool CanAttack();
	bool CanDisarm();
	bool CanArm();
	UFUNCTION(BluePrintCallable)
	void ClearActionState();
	/**
	 * Assassination ability
	 **/
	void InputAssassinate();
	void DisableAllCollision(AActor* Actor);
	UFUNCTION()
	void MoveToAssasinate(float DeltaTime);
	UFUNCTION()
	void PlayAssassinationMontage();
	UFUNCTION(BlueprintCallable)
	void SetCollisonForPawn();
	UFUNCTION(BlueprintCallable)
	void RemoveCollisonForPawn();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
	UAnimMontage* AssassinationMontage;
	UPROPERTY()
	FVector	 EnemyLocation;
	UPROPERTY()
	FRotator EnemyRotation;
	UPROPERTY()
	AActor* TargetToAssassinate;
	bool bIsAssassinating = false;
	/**
	 * Dash
	 **/
	UFUNCTION()
	void InputDash();
	UFUNCTION()
	void StopDash();
	UFUNCTION()
	void ResetDashCooldown();
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashStrength = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDuration = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashCooldown = 1.0f;

	float LastMoveInputTime = -1.f;
	
	bool bCanDash = true;
	bool bIsDashing = false;

	FVector DashDirection;

	FTimerHandle DashStopHandle;
	FTimerHandle DashCooldownHandle;

	
	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "States", meta=(AllowPrivateAccess = "true"))
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "States", meta=(AllowPrivateAccess = "true"))
	EActionState	ActionState = EActionState::EAS_Unoccupied;
	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* StimulusSource;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "UI", meta=(AllowPrivateAccess = "true"))
	float MaxHealth = {100.f};
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "UI", meta=(AllowPrivateAccess = "true"))
	float Health;
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;
	/**
	* Animation Montages
	**/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage", meta=(AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage", meta=(AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	

	/**
	* Interface
	**/
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHealthBar> HealthBarClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	UHealthBar* HealthbarRef;
	

	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int const OtherBodyIndex,bool const FromSweep,FHitResult const &SweepResult);
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void UpdateFallingState();
	void SetupStimulusSource();

	/**
	* Player physics
	**/
	void CollisionFunction(float DeltaTime);
	FVector CalculateNormalForce(FVector Force, FVector Normal);
	void ApplyFriction(double NormalForce);
	void Depenetrate();
	void Accelerate(double NormalForce);
	void Decelerate(float DeltaTime);
	void ApplyVelocity(float DeltaTime);
};
