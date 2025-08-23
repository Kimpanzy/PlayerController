#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped		UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHand UMETA(DisplayName = "EquippedOneHand"),
	ECS_EquippedTwoHand UMETA(DisplayName = "EquippedTwoHand"),
};
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied		UMETA(DisplayName = "noccupied"),
	EAS_Attacking		UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon	UMETA(DisplayName = "Equipping Weapon"),
	EAS_Hanging			UMETA(DisplayName = "Hanging"),
	EAS_Climbing		UMETA(DisplayName = "Climbing"),
	EAS_Assassinating 	UMETA(DisplayName = "Assassinating")
};
UENUM(BlueprintType)
enum class EDetachType : uint8
{
	EDT_Drop		UMETA(DisplayName = "Drop"),
	EDT_PushBack	UMETA(DisplayName = "PushBack"),
	EDT_ClimbUp		UMETA(DisplayName = "ClimbUp"),
	EDT_Jump		UMETA(DisplayName = "Jump"),
};