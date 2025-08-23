
#include "PlayerPawn.h"
#include "PlayerPawn.h"

#include "BrainComponent.h"
#include "Consts.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "Components/BoxComponent.h"
#include "InputActionValue.h"
#include "AI/BaseAI.h"
#include "AI/BaseAIController.h"
#include "Components/CapsuleComponent.h"
#include "AI/BaseParentAI.h"
#include "Engine/OverlapResult.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "UI/HealthBar.h"
#include "Item/Item.h"
#include "Item/Weapon/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/SphereComponent.h"
#include "MotionWarpingComponent.h"

APlayerPawn::APlayerPawn() :
	Health{MaxHealth}
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(34.f, 88.f);
	RootComponent = CapsuleComponent;
	
	
	//SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(
		TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	//Camera
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(
	TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(SpringArm);

	
	//Player Mesh
	VisualMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	VisualMesh->SetupAttachment(RootComponent);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualMesh->SetSimulatePhysics(false);
	VisualMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));

	CapsuleComponent->SetCollisionProfileName("Player");
	CapsuleComponent->SetGenerateOverlapEvents(true);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);

	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	
	
	FistBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("FistBoxComponent"));
	if (FistBoxComponent)
	{
		FAttachmentTransformRules const Rules{
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepRelative,
			false};
		FistBoxComponent->SetupAttachment(VisualMesh,defs::RightFistSocketName);
		FistBoxComponent->SetBoxExtent(defs::RightFistBoxSize);
		FistBoxComponent->SetRelativeLocation(defs::CollisionBoxLocation);
	}
	
	SetupStimulusSource();
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(PlayerInputContext, 0);
		}
	}
	if (IsValid(HealthBarClass))
	{
		if (UHealthBar* Healthbar = CreateWidget<UHealthBar>(GetWorld(),HealthBarClass))
		{
			Healthbar->AddToViewport();
			HealthbarRef = Healthbar;
		}
	}
	//FistBoxComponent->OnComponentBeginOverlap.AddDynamic(this,&APlayerPawn::OnAttackOverlapBegin);
	
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ApplyVelocity(DeltaTime);
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		HeightTrace();
		ForwardTrace();
	}
	if (bIsInterpolatingToHang && ActionState == EActionState::EAS_Hanging)
	{
		RotateHangPos(DeltaTime);
	}
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		MoveCapsuleWithRoot();
	}
	if (bIsAssassinating)
	{
		MoveToAssasinate(DeltaTime);
	}
	if (bIsDashing)
	{
		FVector NewLocation = GetActorLocation() + (DashDirection * DashStrength * DeltaTime);
		SetActorLocation(NewLocation, true);
	}
	MoveForwardAnim(DeltaTime);
	Accelerate(DeltaTime);
	Decelerate(DeltaTime);
	CollisionFunction(DeltaTime);
	Depenetrate();
	Move(Velocity * DeltaTime);
	
	UpdateFallingState();
	
	if (HealthbarRef)
	{
		HealthbarRef->SetHealthBarPercentage(Health/MaxHealth);
	}
	CachedAcceleration = CurrentInput.GetSafeNormal() * Acceleration;
	RotateMeshToMovement(DeltaTime);
	CurrentInput = FVector::ZeroVector;
}
void APlayerPawn::RotateMeshToMovement(float DeltaTime)
{
	FVector MovementDirection = Velocity;
	MovementDirection.Z = 0;

	if (!MovementDirection.IsNearlyZero())
	{
		const FRotator DesiredRotation = MovementDirection.Rotation();
		const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), DesiredRotation, DeltaTime, 10.f);
		SetActorRotation(NewRotation,ETeleportType::None);
	}
}
void APlayerPawn::UpdateFallingState()
{
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);

	FVector TraceStart = Origin;
	FVector TraceEnd = Origin + FVector::DownVector * GroundCheckDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult Hit;
	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		TraceChannelProperty,
		FCollisionShape::MakeBox(Extent),
		QueryParams
	);

	bIsFalling = !bHit;
}
void APlayerPawn::Spin()
{
	Spinner.Broadcast();
	UE_LOG(LogTemp, Warning, TEXT("State: %s"), *UEnum::GetValueAsString(ActionState))
}

void APlayerPawn::OnAttackOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int const OtherBodyIndex,
	bool const FromSweep,
	FHitResult const& SweepResult)
{
	if (OtherActor == this)
	{
		return;
	}
	if (auto const AI = Cast<ABaseParentAI>(OtherActor))
	{
		auto const NewHeath = AI->GetHealth() - GetMaxHealth() * 0.2;
		AI->SetHealth(NewHeath);
	}
}

FVector APlayerPawn::GetMovementSpeed() const
{
	return Velocity;
}

float APlayerPawn::GetMaxHealth() const
{
	return MaxHealth;
}

float APlayerPawn::GetHealth() const
{
	return Health;
}

void APlayerPawn::SetHealth(float const NewHealth)
{
	Health = NewHealth;
	if (Health <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Player is dead"));
	}
}

USkeletalMeshComponent* APlayerPawn::GetMeshComponent() const
{
	return VisualMesh;
}

bool APlayerPawn::GetIsFalling() const
{
	return bIsFalling;
}

void APlayerPawn::SetupStimulusSource()
{
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimulusSource"));
	if (StimulusSource)
	{
		StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimulusSource->RegisterWithPerceptionSystem();
	}
}

void APlayerPawn::CollisionFunction(const float DeltaTime)
{
	int Counter = 0;

	bool bHit;
	do
	{
		const FVector Movement = Velocity * DeltaTime;

		if (Counter == 9)
		{
			UE_LOG(LogTemp, Warning,TEXT("Escape"))
		}
		if (Velocity.Size() < 0.0f)
		{
			Velocity = FVector::ZeroVector;
			return;
		}
		if (Counter >= 10)
		{
			UE_LOG(LogTemp, Warning, TEXT("Escape"))
			Velocity = FVector::ZeroVector;
			return;
		}

		FVector Origin, Extent;
		GetActorBounds(true,Origin,Extent);

		const FVector TraceStart = Origin;
		const FVector TraceEnd = Origin + Movement.GetSafeNormal() *
			(Movement.Size() + ColliderMargin) * 144.f * 10.f;

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		FHitResult Hit;
		bHit = GetWorld()->SweepSingleByChannel(
			Hit,
			TraceStart,
			TraceEnd,
			FQuat::Identity,
			TraceChannelProperty,
			FCollisionShape::MakeCapsule(Extent),
			QueryParams);

		if (bHit)
		{
			const float DistanceToColliderNeg = ColliderMargin / FVector::DotProduct(
				Movement.GetSafeNormal(),
				Hit.Normal);
			const float AllowedMovementDistance = Hit.Distance + DistanceToColliderNeg;
			if (AllowedMovementDistance > Movement.Size())
			{
				return;
			}
			if (AllowedMovementDistance > 0.f)
			{
				Move(Movement.GetSafeNormal() * AllowedMovementDistance);
			}
			else
			{
				FHitResult GroundHit;
				if (GetWorld()->SweepSingleByChannel(
					GroundHit,
					TraceStart,
					Origin - Hit.Normal * (Movement.Size()+ColliderMargin),
					FQuat::Identity,
					TraceChannelProperty,
					FCollisionShape::MakeCapsule(Extent),
					QueryParams))
				{
					Move(-Hit.Normal * (GroundHit.Distance - ColliderMargin));
				}
			}
			const FVector NormalForce = CalculateNormalForce(Velocity,Hit.Normal);
			Velocity += NormalForce;
			ApplyFriction(NormalForce.Size());
			Counter++;
		}
	}
	while (bHit);
}

FVector APlayerPawn::CalculateNormalForce(FVector Force, FVector Normal)
{
	const float Dot = FVector::DotProduct(Force, Normal);
	const FVector Projection = Dot < 0.f ? Dot * Normal : FVector::ZeroVector;
	return -Projection;
}

void APlayerPawn::ApplyFriction(double NormalForce)
{
	if(Velocity.Size() < NormalForce * StaticFrictionCoefficient)
	{
		Velocity = FVector::ZeroVector;
	}
	else
	{
		Velocity -= Velocity.GetSafeNormal() * NormalForce * KineticFrictionCoefficient;
	}
}

void APlayerPawn::Depenetrate()
{
	FVector Origin, Extent;
	GetActorBounds(true,Origin,Extent);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<FOverlapResult> Overlaps;

	if (GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,
		FQuat::Identity,
		TraceChannelProperty,
		FCollisionShape::MakeCapsule(Extent),
		QueryParams))
	{
		for (FOverlapResult& Overlap : Overlaps)
		{
			UE_LOG(LogTemp, Warning, TEXT("Depenetrate Overlap: %s"), *Overlap.Component->GetName());
			FMTDResult MTD;
			if (Overlap.Component->ComputePenetration(
				MTD,
				FCollisionShape::MakeCapsule(Extent+ FVector(ColliderMargin)),
				Origin,
				FQuat::Identity))
			{
				SetActorLocation(GetActorLocation() + MTD.Direction * (MTD.Distance + ColliderMargin));
				const FVector NormalForce = CalculateNormalForce
				(Velocity,-MTD.Direction);
				Velocity += NormalForce;
				ApplyFriction(NormalForce.Size());
				break;
			}
		}
	}
}

void APlayerPawn::Accelerate(double NormalForce)
{
	Velocity += CurrentInput.GetSafeNormal() * Acceleration * NormalForce;
}

void APlayerPawn::Decelerate(float DeltaTime)
{
	FVector HorizontalVelocity = FVector(Velocity.X,Velocity.Y, 0.f);

	if (HorizontalVelocity.IsZero()) return;

	FVector DecelDirection = -HorizontalVelocity.GetSafeNormal();
	float DecelAmount = Deceleration * DeltaTime;
	
	if (HorizontalVelocity.Size() <= DecelAmount)
	{
		Velocity.X = 0.f;
		Velocity.Y = 0.f;
	}
	else
	{
		FVector NewHorizontal = HorizontalVelocity + DecelDirection * DecelAmount;
		Velocity.X = NewHorizontal.X;
		Velocity.Y = NewHorizontal.Y;
	}
}

void APlayerPawn::ApplyVelocity(float DeltaTime)
{
	if (ActionState != EActionState::EAS_Hanging && ActionState != EActionState::EAS_Climbing)
	{
		if (Velocity.Z > 0.f)
		{
			GravityScale = JumpGravityMultiplier;
		}
		else
		{
			GravityScale = FallGravityMultiplier;
		}
		Velocity += FVector::UpVector * GetWorld()->GetGravityZ() * GravityScale * DeltaTime;
	}
	
	Velocity += CurrentInput.GetSafeNormal() * Acceleration * DeltaTime;
	Velocity *= FMath::Pow(AirResistance, DeltaTime);
	
	FVector HorizontalVelocity = FVector(Velocity.X, Velocity.Y, 0.f).GetClampedToMaxSize(MovementSpeed);
	Velocity.X = HorizontalVelocity.X;
	Velocity.Y = HorizontalVelocity.Y;
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&APlayerPawn::InputLook);
		EnhancedInput->BindAction(
			MovementAction,
			ETriggerEvent::Triggered,
			this,
			&APlayerPawn::InputMove);
		EnhancedInput->BindAction(
			SpinAction,
			ETriggerEvent::Started,
			this,
			&APlayerPawn::Spin);
		EnhancedInput->BindAction(
			AttackAction,
			ETriggerEvent::Started,
			this,
			&APlayerPawn::InputAttack);
		EnhancedInput->BindAction(
			InteractAction,
			ETriggerEvent::Started,
			this,
			&APlayerPawn::InputInteract);
		EnhancedInput->BindAction(
			ClimbUpAction,
			ETriggerEvent::Started,
			this,
			&APlayerPawn::InputClimbUp);
		EnhancedInput->BindAction(
			DropAction,
			ETriggerEvent::Started,
			this,
			&APlayerPawn::InputDropDown);
		EnhancedInput->BindAction(
			SprintAction,
			ETriggerEvent::Triggered,
			this,
			&APlayerPawn::InputSprint);
		EnhancedInput->BindAction(
			DashAction,
			ETriggerEvent::Triggered,
			this,
			&APlayerPawn::InputDash);
		EnhancedInput->BindAction(
			SprintAction,
			ETriggerEvent::Completed,
			this,
			&APlayerPawn::InputSprintStop);
		EnhancedInput->BindAction(
			AssassinateAction,
			ETriggerEvent::Triggered,
			this,
			&APlayerPawn::InputAssassinate);
	}
	InputComponent->BindAction(
		"Jump",
		IE_Pressed,
		this,
		&APlayerPawn::JumpInput);
	
}

void APlayerPawn::JumpInput()
{
	
	if (ActionState != EActionState::EAS_Unoccupied) return;
	FVector Origin, Extent;
	GetActorBounds(true,Origin,Extent);

	const FVector TraceStart = Origin;
	const FVector TraceEnd = Origin + FVector::DownVector * GroundCheckDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FHitResult Hit;
	if (GetWorld()->SweepSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		TraceChannelProperty,
		FCollisionShape::MakeBox(Extent),
		QueryParams))
	{
		
		Velocity += FVector::UpVector * JumpHeight;
	}
}

void APlayerPawn::InputSprint()
{
	MovementSpeed = SprintSpeed;
}

void APlayerPawn::InputSprintStop()
{
	MovementSpeed = WalkSpeed;
}

void APlayerPawn::InputDash()
{
	if (!bCanDash) return;
	
	bIsDashing = true;
	bCanDash = false;

	DashDirection = GetActorForwardVector().GetSafeNormal();
	
	GetWorld()->GetTimerManager().SetTimer(DashStopHandle,this, &APlayerPawn::StopDash,DashDuration,false);
	GetWorld()->GetTimerManager().SetTimer(DashCooldownHandle,this, &APlayerPawn::ResetDashCooldown,DashCooldown,false);
}
void APlayerPawn::StopDash()
{
	bIsDashing = false;
}

void APlayerPawn::ResetDashCooldown()
{
	bCanDash = true;
}
void APlayerPawn::InputLook(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(LookAxisValue.X * 1.f);
		AddControllerPitchInput(LookAxisValue.Y * 1.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No Controller!"));
	}
}

void APlayerPawn::InputMove(const FInputActionValue& Value)
{
	
	const FVector2D MoveVector = Value.Get<FVector2D>();
	
	if (!Controller || MoveVector.IsNearlyZero()) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0,Rotation.Yaw,0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const FVector InputVector = FVector(ForwardDirection * MoveVector.Y + RightDirection * MoveVector.X);
	
	
	//Stop movement when attacking
	if (ActionState != EActionState::EAS_Unoccupied) return;

	CurrentInput = InputVector;
}

void APlayerPawn::PlayAttackMontage() const
{
	auto* AnimeInstance = GetMeshComponent()->GetAnimInstance();
	if (AnimeInstance && AttackMontage)
	{
		AnimeInstance->Montage_Play(AttackMontage);
		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
			
		case 1:
			SectionName = FName("Attack2");
			break;
			
		default:
			SectionName = FName("Attack1");
			break;
		}
		AnimeInstance->Montage_JumpToSection(SectionName,AttackMontage);
	}
}

void APlayerPawn::PlayEquipMontage(FName SectionName) const
{
	auto* AnimInstance = GetMeshComponent()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName,EquipMontage);
	}
}

void APlayerPawn::InputAttack()
{
	if (CanAttack())
	{
	PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}
bool APlayerPawn::CanAttack()
{
	
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool APlayerPawn::CanDisarm()
{
	return	ActionState == EActionState::EAS_Unoccupied &&
			CharacterState != ECharacterState::ECS_Unequipped;
			
}

bool APlayerPawn::CanArm()
{
	return	ActionState == EActionState::EAS_Unoccupied &&
			CharacterState == ECharacterState::ECS_Unequipped &&
			EquippedWeapon;
}

void APlayerPawn::ClearActionState()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerPawn::InputAssassinate()
{
	FHitResult HitResult;

	FVector Start = GetActorLocation();
	FVector ForwardDir = GetActorForwardVector() * 1000.f;
	FVector End = Start + ForwardDir;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(50.f);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		EnemyTrace,
		CollisionShape,
		CollisionParams);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (auto* Enemy = Cast<ABaseAI>(HitActor))
		{
			if (auto* AiController = Cast<ABaseAIController>(Enemy->GetInstigatorController()))
			{
				if (AiController->BrainComponent)
				{
					AiController->BrainComponent->StopLogic("Assassinated");
					ActionState = EActionState::EAS_Assassinating;
					
					
					FVector EnemyLoc = Enemy->GetActorLocation() - FVector(0.f, 65.f, 0.f);
					FVector EnemyDir = Enemy->GetActorForwardVector();
					FRotator LookAtEnemy = Enemy->GetActorRotation() - FRotator(0.f, 20.f, 0.f);

					EnemyLocation = EnemyLoc + EnemyDir * -45.f;
					EnemyRotation = LookAtEnemy;

					TargetToAssassinate = Enemy;
					
					bIsAssassinating = true;

				}	
			}
		}
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);
		DrawDebugSphere(GetWorld(), End, 50.f, 12, FColor::Red, false, 2.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f);
		DrawDebugSphere(GetWorld(), End, 50.f, 12, FColor::Green, false, 2.0f);
	}
}
void APlayerPawn::DisableAllCollision(AActor* Actor)
{
	TArray<UPrimitiveComponent*> Components;
	Actor->GetComponents<UPrimitiveComponent>(Components);

	for (auto* Comp : Components)
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Comp->SetGenerateOverlapEvents(false);
	}
}
void APlayerPawn::MoveToAssasinate(float DeltaTime)
{
	FVector NewLocation = FMath::VInterpTo(GetActorLocation(), EnemyLocation, DeltaTime, 15.f);
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), EnemyRotation, DeltaTime, 15.f);

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);

	const float LocationTolerance = 5.0f;
	const float RotationTolerance = 5.0f;

	bool bLocationReached = FVector::Dist(NewLocation, EnemyLocation) < LocationTolerance;
	bool bRotationReached = NewRotation.Equals(EnemyRotation, RotationTolerance);

	if (bLocationReached && bRotationReached)
	{
		PlayAssassinationMontage();
	}
}

void APlayerPawn::PlayAssassinationMontage()
{
	auto* AnimInstance = GetMeshComponent()->GetAnimInstance();
	
	if (AnimInstance && AssassinationMontage)
	{
		if (CharacterState == ECharacterState::ECS_Unequipped)
		{
			AnimInstance->Montage_Play(AssassinationMontage);
			AnimInstance->Montage_JumpToSection("Unequipped",AssassinationMontage);
			if (TargetToAssassinate)
			{
				auto* Target = Cast<ABaseAI>(TargetToAssassinate);
				Target->PlayStrangleMontage();
			}
			UE_LOG(LogTemp, Warning,TEXT("Montage1"))
		}
		else
		{
			AnimInstance->Montage_Play(AssassinationMontage);
			AnimInstance->Montage_JumpToSection("Equipped",AssassinationMontage);
			UE_LOG(LogTemp, Warning,TEXT("Montage2"))
		}
	}
		bIsAssassinating = false;
	
}

void APlayerPawn::SetCollisonForPawn()
{
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	UE_LOG(LogTemp, Warning,TEXT("SetCollisonForPawn"))
}

void APlayerPawn::RemoveCollisonForPawn()
{
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	UE_LOG(LogTemp, Warning,TEXT("RemoveCollisonForPawn"))
}

void APlayerPawn::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMeshComponent(),FName("SpineSocket"));
	}
}

void APlayerPawn::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMeshComponent(),defs::RightFistSocketName);
	}
}

void APlayerPawn::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
	UE_LOG(LogTemp, Warning, TEXT("finish eq"))
}

void APlayerPawn::InputInteract()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(VisualMesh, defs::RightFistSocketName);
		CharacterState = ECharacterState::ECS_EquippedOneHand;
		EquippedWeapon = OverlappingWeapon;
		OverlappingItem = nullptr;
	}
	else
	{
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
			UE_LOG(LogTemp, Warning, TEXT("Unequip"));
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedOneHand;
			ActionState = EActionState::EAS_EquippingWeapon;
			UE_LOG(LogTemp, Warning, TEXT("equip"));
		}
	}
}

void APlayerPawn::AttackStart()
{
	FistBoxComponent->SetCollisionProfileName("Fist");
	FistBoxComponent->SetNotifyRigidBodyCollision(true);
}

void APlayerPawn::AttackEnd()
{
	UE_LOG(LogTemp, Error, TEXT("AttackEnd"));
	ActionState = EActionState::EAS_Unoccupied;
	FistBoxComponent->SetCollisionProfileName("Fist");
	FistBoxComponent->SetNotifyRigidBodyCollision(false);
}

void APlayerPawn::HeightTrace()
{
	FHitResult HitResult;

	FVector Forward = GetActorForwardVector() * 75;
	FVector Start = GetActorLocation() + FVector(0.f,0.f,500.f) + Forward;
	FVector End = Start - FVector(0, 0, 500.f);
	
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(10.f);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		LedgeTrace,
		CollisionShape,
		CollisionParams);
	if (bHit)
	{
		FVector EdgeLocation = HitResult.Location;
		FVector SocketLocation = VisualMesh->GetSocketLocation("SpineSocket_0");

		float InRange = EdgeLocation.Z - SocketLocation.Z;
		FVector WallNormalDirection = WallNormal * 35.f;
		FVector WallPosition = WallLocation + WallNormalDirection;
		
		FRotator Facewall = FRotationMatrix::MakeFromX(WallNormal * -1).Rotator();
		
		FVector DesiredLocation = FVector(WallPosition.X,WallPosition.Y,EdgeLocation.Z - 70.f);

		if (InRange <= 75 && InRange >= 0  && ActionState != EActionState::EAS_Hanging)
		{
			ActionState = EActionState::EAS_Hanging;
			Velocity = FVector::ZeroVector;

			TargetHangLocation = DesiredLocation;
			TargetHangRotation = Facewall;

			bIsInterpolatingToHang = true; 
		}
		
		/*DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 12, FColor::Red, false, 2.0f);
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f);
		DrawDebugSphere(GetWorld(), End, 10.f, 12, FColor::Green, false, 2.0f);
	}*/
	}
}

void APlayerPawn::ForwardTrace()
{
	FHitResult HitResult;

	FVector Forward = GetActorForwardVector() * 100;
	FVector Start = GetActorLocation();
	FVector End = Start + Forward;

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(10.f);

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		LedgeTrace,
		CollisionShape,
		CollisionParams);

	if (bHit)
	{
		WallNormal = HitResult.Normal;
		WallLocation = HitResult.Location;
		//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 12, FColor::Red, false, 2.0f);
	}
	/*else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f);
		DrawDebugSphere(GetWorld(), End, 10.f, 12, FColor::Green, false, 2.0f);
	}*/
}
void APlayerPawn::PlayClimbingMontage()
{
	auto* AnimInstance = VisualMesh->GetAnimInstance();
	if (AnimInstance && ClimbUpMontage)
	{
		if (!AnimInstance->Montage_IsPlaying(ClimbUpMontage))
		{
			ActionState = EActionState::EAS_Climbing;
			AnimInstance->Montage_Play(ClimbUpMontage);
			
		}
	}
}

void APlayerPawn::RotateHangPos(float DeltaTime)
{
	FVector NewLocation = FMath::VInterpTo(GetActorLocation(),TargetHangLocation, DeltaTime,10.f);
	FRotator NewRotation = FMath::RInterpTo(GetActorRotation(),TargetHangRotation,DeltaTime,5.f);

	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);

	const float LocationTolerance = 1.0f;
	const float RotationTolerance = 1.0f;

	bool bLocationReached = FVector::Dist(NewLocation, TargetHangLocation) < LocationTolerance;
	bool bRotationReached = NewRotation.Equals(TargetHangRotation, RotationTolerance);

	if (bLocationReached && bRotationReached)
	{
		bIsInterpolatingToHang = false;
	}
}

void APlayerPawn::InputDropDown()
{
	if (ActionState != EActionState::EAS_Hanging) return;

	ActionState = EActionState::EAS_Unoccupied;
	UE_LOG(LogTemp, Warning, TEXT("DropDown"));
}

void APlayerPawn::InputClimbUp()
{
	if (ActionState != EActionState::EAS_Hanging) return;
	PlayClimbingMontage();
	
}

void APlayerPawn::FinishClimb()
{
	bAnimDrivenMoveForward = false;
	ActionState = EActionState::EAS_Unoccupied;
}

void APlayerPawn::MoveCapsuleWithRoot()
{
	if (ActionState != EActionState::EAS_Unoccupied)
	{
		if (VisualMesh && VisualMesh->GetAnimInstance())
		{
			FRootMotionMovementParams RootMotion = VisualMesh->ConsumeRootMotion();

			if (RootMotion.bHasRootMotion)
			{
				FTransform RootMotionTransform = RootMotion.GetRootMotionTransform();
				FVector DeltaLocation = RootMotionTransform.GetTranslation();
				
				AddActorWorldOffset(DeltaLocation, true);
			}
		}
	}
}

void APlayerPawn::MoveForwardAnim(float Deltatime)
{
	if (!bAnimDrivenMoveForward) return;
	
	FVector MoveTo = GetActorLocation()+GetActorForwardVector()*30.f;
	FVector NewLocation = FMath::VInterpTo(GetActorLocation(),MoveTo,Deltatime,5.f);
	SetActorLocation(NewLocation);
	if (FVector::Dist(NewLocation,MoveTo) < 1.f)
	{
		bAnimDrivenMoveForward = false;
	}
}

void APlayerPawn::SetMovingForwardBool()
{
	bAnimDrivenMoveForward = true;
}

void APlayerPawn::Move(FVector Movement)
{
	SetActorLocation(GetActorLocation() + Movement);
	
}

