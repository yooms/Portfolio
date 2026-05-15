#include "Character/Animation/KMBaseAnimInstance.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"
#include "DrawDebugHelpers.h"
#include "CharacterTrajectoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PoseSearch/AnimNode_MotionMatching.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "PoseSearch/PoseSearchTrajectoryLibrary.h"



void UKMBaseAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<AKMCharacter>(TryGetPawnOwner());
    if (IsValid(Character))
    {
        MovementComponent = Character->GetCharacterMovement();
        TrajectoryComponent = Character->GetCharacterTrajectoryComponent();
    }
}

void UKMBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!IsValid(Character) || !IsValid(MovementComponent))
        return;

    Velocity = MovementComponent->Velocity;
    Acceleration = MovementComponent->GetCurrentAcceleration();

    GroundSpeed = Velocity.Size2D();
    ShouldMove = GroundSpeed > 3.f && !Acceleration.IsZero();
    IsFalling = MovementComponent->IsFalling();
    AimPitch = Character->GetAimPitch();
    AimYaw = Character->GetAimYaw();

    if (!IsFalling)
    {
        Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
        DirectionIndex = CalculateDirection8Way(Direction);

        UpdateTurnInPlace(DeltaSeconds);

        UpdateFootIK(DeltaSeconds);
    }
}

void UKMBaseAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

    if (!IsValid(Character) || !IsValid(MovementComponent))
        return;
/*
    FPoseSearchTrajectoryData TrajData;
    TrajData.RotateTowardsMovementSpeed = RotateTowardsMovementSpeed;
    TrajData.MaxControllerYawRate = (StrafeMode == EKMStrafeMode::Strafe) ? MaxControllerYawRate : -1.f;
    TrajData.BendVelocityTowardsAcceleration = BendVelocityTowardsAcceleration;
    TrajData.bUseSpeedRemappingCurve = false;
    TrajData.bUseAccelerationRemappingCurve = false;

    FPoseSearchQueryTrajectory OutTrajectory;
    UPoseSearchTrajectoryLibrary::PoseSearchGenerateTrajectory(this, 
                                                                TrajData, 
                                                                DeltaSeconds, 
                                                                Trajectory, 
                                                                DesiredControllerYawLastUpdate, 
                                                                OutTrajectory, 
                                                                TrajectorySettings.HistorySamplingInterval, 
                                                                TrajectorySettings.TrajectoryHistoryCount, 
                                                                TrajectorySettings.PredictionSamplingInterval, 
                                                                TrajectorySettings.TrajectoryPredictionCount);

    Trajectory = OutTrajectory;
*/
}

bool UKMBaseAnimInstance::IsMoving() const
{
    const bool VelocityValue = Velocity.Length() != 0.1f;
    const bool AccelerationValue = Acceleration.Length() != 0.f;

    if (VelocityValue && AccelerationValue)
    {
        return true;
    }

    return false;
}

bool UKMBaseAnimInstance::IsStarting() const
{
    const bool Moving = IsMoving();
    const bool Vel = Trj_FutureVelocity.Size2D() >= Velocity.Size2D() + 100.f;
    const bool FindPivot = CurrentDatabaseTags.Contains(FName("Pivot"));

    if (Moving && Vel && FindPivot)
    {
        return true;
    }

    return false;
}

bool UKMBaseAnimInstance::IsPivoting() const
{


    return false;
}

float UKMBaseAnimInstance::Get_TrajectoryTurnAngle() const
{
    const FRotator AccelerationRotation = FRotationMatrix::MakeFromX(Acceleration).Rotator();
    const FRotator VelocityRotation = FRotationMatrix::MakeFromX(Velocity).Rotator();
    const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(AccelerationRotation, VelocityRotation);

    return DeltaRot.Yaw;
}

int32 UKMBaseAnimInstance::CalculateDirection8Way(float InDirection)
{
    // -180 ~ 180 → 8방향 분할
    if (InDirection >= -22.5f && InDirection < 22.5f) return 0;      // Forward
    if (InDirection >= 22.5f && InDirection < 67.5f) return 1;       // Forward Right
    if (InDirection >= 67.5f && InDirection < 112.5f) return 2;      // Right
    if (InDirection >= 112.5f && InDirection < 157.5f) return 3;     // Back Right
    if (InDirection >= 157.5f || InDirection < -157.5f) return 4;    // Back
    if (InDirection >= -157.5f && InDirection < -112.5f) return 5;   // Back Left
    if (InDirection >= -112.5f && InDirection < -67.5f) return 6;    // Left
    if (InDirection >= -67.5f && InDirection < -22.5f) return 7;     // Forward Left

    return 0;
}

void UKMBaseAnimInstance::UpdateTurnInPlace(float DeltaSeconds)
{
    FRotator ControlRot = Character->GetControlRotation();
    FRotator ActorRot = Character->GetActorRotation();

    FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, ActorRot);
    AimYaw = Delta.Yaw;

    InterpAimYaw = FMath::FInterpTo(InterpAimYaw, AimYaw, DeltaSeconds, 6.f);

    bTurningInPlace = FMath::Abs(AimYaw) > 60.f && GroundSpeed < 10.f;
}

void UKMBaseAnimInstance::UpdateFootIK(float DeltaSeconds)
{
    if (!Character)
        return;

    FVector ActorLocation = Character->GetActorLocation();

    auto TraceFoot = [&](FName SocketName, float& OutOffset, FRotator& OutRot)
        {
            FVector SocketLoc = Character->GetMesh()->GetSocketLocation(SocketName);

            FVector Start = SocketLoc;
            FVector End = Start - FVector(0, 0, 50.f);

            FHitResult Hit;
            FCollisionQueryParams Params;
            Params.AddIgnoredActor(Character);

            bool bHit = GetWorld()->LineTraceSingleByChannel(
                Hit, Start, End, ECC_Visibility, Params
            );

            if (bHit)
            {
                float TargetOffset = (Hit.Location - End).Z;

                OutOffset = FMath::FInterpTo(OutOffset, TargetOffset, DeltaSeconds, 15.f);

                FRotator TargetRot = UKismetMathLibrary::MakeRotFromZX(Hit.Normal, Character->GetActorForwardVector());
                OutRot = FMath::RInterpTo(OutRot, TargetRot, DeltaSeconds, 15.f);
            }
            else
            {
                OutOffset = FMath::FInterpTo(OutOffset, 0.f, DeltaSeconds, 15.f);
                OutRot = FMath::RInterpTo(OutRot, FRotator::ZeroRotator, DeltaSeconds, 15.f);
            }
        };

    TraceFoot(TEXT("foot_l"), LeftFootOffset, LeftFootRotation);
    TraceFoot(TEXT("foot_r"), RightFootOffset, RightFootRotation);
}

void UKMBaseAnimInstance::UpdateMotionMatching()
{

}

UPoseSearchDatabase* UKMBaseAnimInstance::GetActiveDatabase() const
{
    switch (LocomotionState)
    {
    case EKMLocomotionState::InAir:
        {
            if (IsValid(InAirDatabase))
            {
                return InAirDatabase;
            }
        }
        break;
    case EKMLocomotionState::Crouch:
        {
            if (IsValid(CrouchDatabase))
            {
                return CrouchDatabase;
            }
        }
        break;
    case EKMLocomotionState::Run:
        {
            if (IsValid(RunDatabase))
            {
                return RunDatabase;
            }
        }
        break;
    case EKMLocomotionState::Walk:
        {
            if (IsValid(WalkDatabase))
            {
                return WalkDatabase;
            }
        }
        break;
    default:
        break;
    }
    return LocomotionDatabase;
}

