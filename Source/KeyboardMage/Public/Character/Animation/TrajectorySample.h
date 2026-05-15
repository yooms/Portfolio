#pragma once

#include "CoreMinimal.h"
#include "TrajectorySample.generated.h"


USTRUCT(BlueprintType)
struct FTrajectoryPoint
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Position = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Facing = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Time = 0.f;
};
