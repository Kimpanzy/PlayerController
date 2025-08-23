// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HealthBar.h"

void UHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHealthBar::SetHealthBarPercentage(float const Value)
{
	HealthValue->SetPercent(Value);
}
