// PoseDatabase.h

#pragma once

#include "CoreMinimal.h"
#include "PoseFeature.h"
#include "Animation/AnimSequence.h"
#include "PoseDatabase.generated.h"

USTRUCT()
struct FKMPoseData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FPoseFeature Feature;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UAnimSequence> Animation;

    UPROPERTY(EditAnywhere)
    float Time = 0.f;
};

UCLASS(Blueprintable)
class KEYBOARDMAGE_API UPoseDatabase : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    TArray<FKMPoseData> Poses;

    FKMPoseData FindBestMatch(const FPoseFeature& Query);
};