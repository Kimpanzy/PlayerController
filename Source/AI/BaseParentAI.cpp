#include "AI/BaseParentAI.h"
#include "Consts.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "PlayerController/PlayerPawn.h"
#include "UI/HealthBar.h"

ABaseParentAI::ABaseParentAI() :
	WidgetComponent(CreateDefaultSubobject<UWidgetComponent>("HealthValue")),
	Health{MaxHealth},
	RightFistBox{CreateDefaultSubobject<UBoxComponent>(TEXT("RightFistBox"))}

{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Setup mesh
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	
	if (WidgetComponent)
	{
		
		WidgetComponent->SetupAttachment(RootComponent);
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponent->SetRelativeLocation(defs::HealthBarZ);
		static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass{TEXT("/Game/UI/BP_HealthBar")};
		if (WidgetClass.Succeeded())
		{
			WidgetComponent->SetWidgetClass(WidgetClass.Class);
		}
	}
	if (RightFistBox)
	{
		FAttachmentTransformRules const Rules{
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepRelative,
		false};
		RightFistBox->SetupAttachment(GetMesh(),defs::RightFistSocketName);
		RightFistBox->SetBoxExtent(defs::RightFistBoxSize);
		RightFistBox->SetRelativeLocation(defs::CollisionBoxLocation);
	}
	
}

void ABaseParentAI::BeginPlay()
{
	Super::BeginPlay();
	RightFistBox->OnComponentBeginOverlap.AddDynamic(this,&ABaseParentAI::OnAttackOverlapBegin);
	RightFistBox->OnComponentEndOverlap.AddDynamic(this,&ABaseParentAI::OnAttackOverlapEnd);
	
}

void ABaseParentAI::OnAttackOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int const OtherBodyIndex,
	bool const FromSweep,
	FHitResult const& SweepResult)
{
	if (bHasHitPlayer)
	{
		return;
	}
	if (OtherActor == this)
	{
		return;
	}
	if (auto const Player = Cast<APlayerPawn>(OtherActor))
	{
		auto const NewHealth = Player->GetHealth() - Player->GetMaxHealth() * 0.1f;
		Player->SetHealth(NewHealth);
		bHasHitPlayer = true;
	}
}

void ABaseParentAI::OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int const OtherBodyIndex)
{
	
}

void ABaseParentAI::Tick(float DeltaTime)
{
	if (auto* const Widget = Cast<UHealthBar>(WidgetComponent->GetUserWidgetObject()))
	{
		Widget->SetHealthBarPercentage(Health/MaxHealth);
	}
	Super::Tick(DeltaTime);

}

void ABaseParentAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseParentAI::GetMaxHealth()const
{
	return MaxHealth;
}

float ABaseParentAI::GetHealth()const
{
	return Health;
	
}

void ABaseParentAI::SetHealth(float const NewHealth)
{
	Health = NewHealth;
	if (Health <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AI is dead"));
	}
}

void ABaseParentAI::AttackStart() const
{
	RightFistBox->SetCollisionProfileName("Fist");
	RightFistBox->SetNotifyRigidBodyCollision(true);
}

void ABaseParentAI::AttackEnd() const
{
	RightFistBox->SetCollisionProfileName("Fist");
	RightFistBox->SetNotifyRigidBodyCollision(false);
	bHasHitPlayer = false;
}

