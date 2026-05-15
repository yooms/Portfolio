#include "Character/Animation/KMAnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/KMCharacter.h"


void UKMAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    if (APawn* Pawn = TryGetPawnOwner())
    {
        PreviousLocation = Pawn->GetActorLocation();
    }
}

void UKMAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (DeltaSeconds <= KINDA_SMALL_NUMBER) return;

//    UpdateTrajectory(DeltaSeconds);

    UpdateSmoothedAimOffset(DeltaSeconds);
}

void UKMAnimInstance::UpdateTrajectory(float DeltaSeconds)
{
    APawn* Pawn = TryGetPawnOwner();
    if (!Pawn) return;

    FVector CurrentLocation = Pawn->GetActorLocation();

    Velocity = (CurrentLocation - PreviousLocation) / DeltaSeconds;
    PreviousLocation = CurrentLocation;

    FRotator ControlRot = Pawn->GetControlRotation();

    const float YawDelta = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, PrevControlRot).Yaw;
    const float TurnSpeed = (DeltaSeconds > 0.f) ? (YawDelta / DeltaSeconds) : 0.f;
    PrevControlRot = ControlRot;

    TrajectoryPoint.Empty(6);

    for (float T = 0.1f; T <= 0.6f; T += 0.1f)
    {
        FTrajectoryPoint Point;
        Point.Time = T;
        Point.Position = CurrentLocation + Velocity * T;

        const float FutureYaw = ControlRot.Yaw + TurnSpeed * T;
        Point.Facing = FRotator(0.f, FutureYaw, 0.f).Vector();

        TrajectoryPoint.Add(Point);
    }
}

void UKMAnimInstance::UpdateSmoothedAimOffset(float DeltaSeconds)
{
    AKMCharacter* KMChar = Cast<AKMCharacter>(TryGetPawnOwner());
    if (!KMChar) return;

    bAimOffsetEnabled = true;
    SmoothedAimPitch = FMath::FInterpTo(SmoothedAimPitch, AimPitch, DeltaSeconds, AimInterpSpeed);
    SmoothedAimYaw = FMath::FInterpTo(SmoothedAimYaw, InterpAimYaw, DeltaSeconds, AimInterpSpeed);
}
