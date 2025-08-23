#pragma once

#include "Math/Vector.h"
namespace defs
{
	FVector const HealthBarZ {0.f,0.f,95.f};
	FVector const RightFistBoxSize {5.f};
	FVector const CollisionBoxLocation {-7.f,0.f,0.f};
	FName const RightFistSocketName{"hand_r_socket"};
	inline constexpr ECollisionChannel CollisionPlayer = ECC_GameTraceChannel1;
}
