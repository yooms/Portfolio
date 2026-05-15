#pragma once

#include "CoreMinimal.h"
#include "KMMotionMatching.generated.h"



UENUM(BlueprintType)
enum class EKMLocomotionState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Walk        UMETA(DisplayName = "Walk"),
    Run         UMETA(DisplayName = "Run"),
    Crouch      UMETA(DisplayName = "Crouch"),
    InAir       UMETA(DisplayName = "InAir"),
};


UENUM(BlueprintType)
enum class EKMStrafeMode : uint8
{
    OrientToMovement UMETA(DisplayName = "Orient To Movement"),
    Strafe           UMETA(DisplayName = "Strafe"),
};

USTRUCT(BlueprintType)
struct FKMTrajectorySettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory", meta = (ClampMin = "0.01"))
    float HistorySamplingInterval = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory", meta = (ClampMin = "1"))
    int32 TrajectoryHistoryCount = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory", meta = (ClampMin = "0.01"))
    float PredictionSamplingInterval = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory", meta = (ClampMin = "1"))
    int32 TrajectoryPredictionCount = 6;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory")
    bool bFlattenTrajectory = true;
};