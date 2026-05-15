#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAttributeSet/KMCharacterAttributeSet.h"
#include "Abilities/GameplayAbility.h"
#include "KMCharacterGameplayAbility.generated.h"

UCLASS()
class KEYBOARDMAGE_API UKMCharacterGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UKMCharacterGameplayAbility();

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> DamageEffectClass;

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};