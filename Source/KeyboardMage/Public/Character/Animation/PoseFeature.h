// PoseFeature.h

#pragma once

#include "CoreMinimal.h"
#include "TrajectorySample.h"
#include "PoseFeature.generated.h"

USTRUCT()
struct FPoseFeature
{
    GENERATED_BODY()

    UPROPERTY()
    FVector RootPosition = FVector::ZeroVector;

    UPROPERTY()
    FVector Velocity = FVector::ZeroVector;

    UPROPERTY()
    TArray<FVector> BonePositions;

    UPROPERTY()
    TArray<FTrajectoryPoint> Trajectory;
};