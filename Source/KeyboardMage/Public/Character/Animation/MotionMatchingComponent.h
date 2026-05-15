// MotionMatchingComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoseDatabase.h"
#include "MotionMatchingComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KEYBOARDMAGE_API UMotionMatchingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    void UpdateMotionMatching(float DeltaTime);

    UPROPERTY(EditAnywhere)
    UPoseDatabase* Database;

private:
    FPoseFeature BuildQuery();
};