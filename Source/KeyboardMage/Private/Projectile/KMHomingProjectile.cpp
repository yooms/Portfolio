#include "Projectile/KMHomingProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"

AKMHomingProjectile::AKMHomingProjectile()
{
	bReplicates = true;

	if (Movement)
	{
		Movement->bIsHomingProjectile  = true;
		Movement->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
	}
}

void AKMHomingProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HomingDuration > 0.f && HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				HomingTimerHandle,
				this,
				&AKMHomingProjectile::DisableHoming,
				HomingDuration,
				false);
		}
	}
}

void AKMHomingProjectile::SetHomingTarget(USceneComponent* TargetComponent)
{
	if (!Movement) return;

	if (TargetComponent)
	{
		Movement->HomingTargetComponent = TargetComponent;
		Movement->bIsHomingProjectile   = true;
		Movement->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
	}
	else
	{
		Movement->bIsHomingProjectile = false;
	}
}

void AKMHomingProjectile::DisableHoming()
{
	if (Movement)
	{
		Movement->bIsHomingProjectile = false;
	}
}
