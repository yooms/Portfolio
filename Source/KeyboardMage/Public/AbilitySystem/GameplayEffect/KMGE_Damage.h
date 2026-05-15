#pragma once


#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "KMGE_Damage.generated.h"

UCLASS()
class KEYBOARDMAGE_API UKMGE_Damage : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UKMGE_Damage()
    {
        DurationPolicy = EGameplayEffectDurationType::HasDuration;
    }
};