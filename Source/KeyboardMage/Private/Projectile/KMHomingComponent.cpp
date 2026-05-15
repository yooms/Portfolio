#include "Projectile/KMHomingComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"


UKMHomingComponent::UKMHomingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UKMHomingComponent::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement =
		GetOwner()->FindComponentByClass<UProjectileMovementComponent>();

	if (!ProjectileMovement)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("ProjectileMovementComponent Missing"));

		return;
	}

	SpawnLocation = GetOwner()->GetActorLocation();
	PreviousLocation = SpawnLocation;

	ProjectileMovement->bIsHomingProjectile = false;
}

void UKMHomingComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(
		DeltaTime,
		TickType,
		ThisTickFunction);

	if (!bEnableHoming)
	{
		return;
	}

	UpdateTravelDistance();

	if (CurrentTravelDistance >= MaxTravelDistance)
	{
		GetOwner()->Destroy();
		return;
	}

	if (!bHomingActivated &&
		CurrentTravelDistance >= HomingStartDistance)
	{
		ActivateHoming();
	}

	if (!bHomingActivated)
	{
		return;
	}

	if (!TargetActor || !IsValid(TargetActor))
	{
		if (bAutoFindTarget)
		{
			FindTarget();
		}

		return;
	}

	if (ProjectileMovement)
	{
		ProjectileMovement->HomingTargetComponent =
			TargetActor->GetRootComponent();
	}
}

void UKMHomingComponent::UpdateTravelDistance()
{
	FVector CurrentLocation =
		GetOwner()->GetActorLocation();

	CurrentTravelDistance +=
		FVector::Distance(
			PreviousLocation,
			CurrentLocation);

	PreviousLocation = CurrentLocation;
}

void UKMHomingComponent::ActivateHoming()
{
	bHomingActivated = true;

	if (bAutoFindTarget)
	{
		FindTarget();
	}

	if (TargetActor)
	{
		SetTarget(TargetActor);
	}
}

void UKMHomingComponent::FindTarget()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	FVector MyLocation =
		GetOwner()->GetActorLocation();

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(
		World,
		APawn::StaticClass(),
		FoundActors);

	float ClosestDistSq = FLT_MAX;

	AActor* ClosestTarget = nullptr;

	for (AActor* Actor : FoundActors)
	{
		if (Actor == GetOwner())
		{
			continue;
		}

		float DistSq =
			FVector::DistSquared(
				MyLocation,
				Actor->GetActorLocation());

		if (DistSq < SearchRadius * SearchRadius &&
			DistSq < ClosestDistSq)
		{
			ClosestDistSq = DistSq;
			ClosestTarget = Actor;
		}
	}

	if (ClosestTarget)
	{
		SetTarget(ClosestTarget);
	}
}

void UKMHomingComponent::SetTarget(AActor* NewTarget)
{
	if (!ProjectileMovement)
	{
		return;
	}

	TargetActor = NewTarget;

	if (!TargetActor)
	{
		ProjectileMovement->bIsHomingProjectile = false;
		return;
	}

	ProjectileMovement->HomingTargetComponent =
		TargetActor->GetRootComponent();

	ProjectileMovement->HomingAccelerationMagnitude =
		HomingAcceleration;

	ProjectileMovement->bIsHomingProjectile = true;
}

void UKMHomingComponent::EnableHoming()
{
	bEnableHoming = true;
}

void UKMHomingComponent::DisableHoming()
{
	bEnableHoming = false;

	if (ProjectileMovement)
	{
		ProjectileMovement->bIsHomingProjectile = false;
	}
}

