#pragma once

#include "CoreMinimal.h"
#include "KMBaseAnimInstance.h"
#include "TrajectorySample.h"
#include "KMAnimInstance.generated.h"


class AKMCharacter;

UCLASS()
class KEYBOARDMAGE_API UKMAnimInstance : public UKMBaseAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
    UPROPERTY(BlueprintReadWrite, Category="KMMotionMatching")
    TArray<FTrajectoryPoint> TrajectoryPoint;

    UPROPERTY(BlueprintReadOnly, Category="AimOffset")
    bool bAimOffsetEnabled = false;

    UPROPERTY(BlueprintReadOnly, Category="AimOffset")
    float SmoothedAimPitch = 0.f;

    UPROPERTY(BlueprintReadOnly, Category="AimOffset")
    float SmoothedAimYaw = 0.f;

private:
    void UpdateTrajectory(float DeltaSeconds);
    void UpdateSmoothedAimOffset(float DeltaSeconds);

private:
    FVector PreviousLocation;
    FRotator PrevControlRot;
    static constexpr float AimInterpSpeed = 15.f;
};