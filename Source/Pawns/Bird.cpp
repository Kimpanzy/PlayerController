
#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	BirdCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BirdCapsule"));
	BirdCapsule->SetCapsuleRadius(20.f);
	BirdCapsule->SetCapsuleRadius(15.f);
	SetRootComponent(BirdCapsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	BirdMesh->SetupAttachment(GetRootComponent());
	
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

