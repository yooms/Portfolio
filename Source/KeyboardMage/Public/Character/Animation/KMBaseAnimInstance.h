// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PoseSearch/PoseSearchDatabase.h"
#include "PoseSearch/PoseSearchLibrary.h"
#include "PoseSearch/PoseSearchTrajectoryTypes.h"   // FPoseSearchQueryTrajectory
#include "PoseSearch/PoseSearchTrajectoryLibrary.h" // UPoseSearchTrajectoryLibrary
#include "Character/KMCharacter.h"
#include "Common/KMMotionMatching.h"
#include "KMBaseAnimInstance.generated.h"



UCLASS()
class KEYBOARDMAGE_API UKMBaseAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
    virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

public:

private:
    UPROPERTY(Transient)
    TObjectPtr<AKMCharacter> Character;

    UPROPERTY(Transient)
    TObjectPtr<UCharacterMovementComponent> MovementComponent;

    UPROPERTY(Transient)
    TObjectPtr<UCharacterTrajectoryComponent>  TrajectoryComponent;

    UPROPERTY(Transient)
    float DesiredControllerYawLastUpdate = 0.f;

    UPROPERTY(Transient)
    TObjectPtr<UPoseSearchDatabase> PreviousDatabase;

protected:
    UPROPERTY(BlueprintReadWrite, Category="KMMotioinMatching")
    FVector Velocity;

    UPROPERTY(BlueprintReadWrite, Category="KMMotioinMatching")
    FVector Acceleration;

    UPROPERTY(BlueprintReadWrite, Category="KMMotioinMatching")
    float Speed;

    UPROPERTY(BlueprintReadWrite, Category="KMMotioinMatching")
    FVector Trj_FutureVelocity;

    UPROPERTY(BlueprintReadWrite, Category="KMMotioinMatching")
    TArray<FName> CurrentDatabaseTags;
    UPROPERTY(BlueprintReadWrite, Category="KMAnimation")
    float GroundSpeed = 0.f;

    UPROPERTY(BlueprintReadWrite, Category="KMAnimation")
    float Direction = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="Movement")
    int32 DirectionIndex; // 0~7 (8πÊ«‚)

    UPROPERTY(BlueprintReadWrite, Category="KMAnimation")
    bool ShouldMove = false;

    UPROPERTY(BlueprintReadWrite, Category="KMAnimation")
    bool IsFalling = false;

    UPROPERTY(BlueprintReadOnly, Category="Turn")
    float AimPitch;

    UPROPERTY(BlueprintReadOnly, Category="Turn")
    float AimYaw;

    UPROPERTY(BlueprintReadOnly, Category="Turn")
    float InterpAimYaw;

    UPROPERTY(BlueprintReadOnly, Category="Turn")
    bool bTurningInPlace;

    UPROPERTY(BlueprintReadOnly, Category="IK")
    float LeftFootOffset;

    UPROPERTY(BlueprintReadOnly, Category="IK")
    float RightFootOffset;

    UPROPERTY(BlueprintReadOnly, Category="IK")
    FRotator LeftFootRotation;

    UPROPERTY(BlueprintReadOnly, Category="IK")
    FRotator RightFootRotation;

    UPROPERTY(EditAnywhere, Category = "Motion Matching")
    TObjectPtr<UPoseSearchDatabase> MotionMatchingDatabase;

    UPROPERTY(BlueprintReadOnly, Category = "Motion Matching")
    FPoseSearchBlueprintResult CurrentSearchResult;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Database")
    TObjectPtr<UPoseSearchDatabase> LocomotionDatabase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Database")
    TObjectPtr<UPoseSearchDatabase> WalkDatabase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Database")
    TObjectPtr<UPoseSearchDatabase> RunDatabase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Database")
    TObjectPtr<UPoseSearchDatabase> CrouchDatabase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Database")
    TObjectPtr<UPoseSearchDatabase> InAirDatabase;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Trajectory")
    float RotateTowardsMovementSpeed = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Trajectory")
    float MaxControllerYawRate = 360.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Trajectory", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BendVelocityTowardsAcceleration = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Matching|Debug")
    FTransformTrajectory Trajectory;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Motion Matching|Trajectory")
    FKMTrajectorySettings TrajectorySettings;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Matching|Debug")
    EKMLocomotionState LocomotionState = EKMLocomotionState::Idle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Motion Matching|Debug")
    EKMStrafeMode StrafeMode = EKMStrafeMode::OrientToMovement;



protected:
    UFUNCTION(BlueprintCallable, Category="KMMotioinMatching")
    bool IsMoving() const;

    UFUNCTION(BlueprintCallable, Category="KMMotioinMatching")
    bool IsStarting() const;

    UFUNCTION(BlueprintCallable, Category="KMMotioinMatching")
    bool IsPivoting() const;

    UFUNCTION(BlueprintCallable, Category="KMMotionMatching")
    float Get_TrajectoryTurnAngle() const;

private:
    int32 CalculateDirection8Way(float InDirection);
    void UpdateTurnInPlace(float DeltaSeconds);
    void UpdateFootIK(float DeltaSeconds);
    void UpdateMotionMatching();

public:
    UFUNCTION(BlueprintCallable, Category = "KMMotionMatching")
    UPoseSearchDatabase* GetActiveDatabase() const;
};
