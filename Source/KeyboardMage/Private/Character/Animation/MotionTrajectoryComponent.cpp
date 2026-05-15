#include "Character/Animation/MotionTrajectoryComponent.h"
#include "GameFramework/Character.h"


UMotionTrajectoryComponent::UMotionTrajectoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UMotionTrajectoryComponent::BeginPlay()
{
    Super::BeginPlay();

    if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
    {
        PreviousLocation = Char->GetActorLocation();
    }
}

void UMotionTrajectoryComponent::UpdateTrajectory(float DeltaTime)
{
    ACharacter* Char = Cast<ACharacter>(GetOwner());
    if (!Char || DeltaTime <= 0.f) return;

    FVector CurrentLocation = Char->GetActorLocation();
    FVector Velocity = (CurrentLocation - PreviousLocation) / DeltaTime;

    PreviousLocation = CurrentLocation;

    TrajectoryPoints.Empty();

    for (float T = 0.1f; T <= 1.0f; T += 0.2f)
    {
        FTrajectoryPoint Point;
        Point.Time = T;
        Point.Position = CurrentLocation + Velocity * T;
        Point.Facing = Velocity;

        TrajectoryPoints.Add(Point);
    }
}