#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrajectorySample.h"
#include "MotionTrajectoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KEYBOARDMAGE_API UMotionTrajectoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMotionTrajectoryComponent();

    virtual void BeginPlay() override;
    void UpdateTrajectory(float DeltaTime);

    const TArray<FTrajectoryPoint>& GetTrajectory() const { return TrajectoryPoints; }

private:
    TArray<FTrajectoryPoint> TrajectoryPoints;
    FVector PreviousLocation;
};