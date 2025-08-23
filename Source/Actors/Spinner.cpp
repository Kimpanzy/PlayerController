// Fill out your copyright notice in the Description page of Project Settings.


#include "Spinner.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerController/PlayerPawn.h"

// Sets default values
ASpinner::ASpinner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->SetupAttachment(RootComponent);
}

void ASpinner::ToggleSpinner()
{
	bIsSpinning = !bIsSpinning;
}

// Called when the game starts or when spawned
void ASpinner::BeginPlay()
{
	Super::BeginPlay();
	APlayerPawn* Player = Cast<APlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player cast to APlayerPawn failed"));
	}
	else
	{
		Player->Spinner.AddUObject(this, &ASpinner::ToggleSpinner);
	}
}

// Called every frame
void ASpinner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsSpinning)
	{
		FRotator CurrentRotation = FRotator(
			RotationSpeed * DeltaTime,
			RotationSpeed * DeltaTime,
			0.f);
		AddActorLocalRotation(CurrentRotation);
	}
}

