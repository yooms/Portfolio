#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/GameplayAbility/KMCharacterGameplayAbility.h"
#include "KMGA_Cost.generated.h"


class UKMGameplayAbility;


UCLASS(MinimalAPI, DefaultToInstanced, EditInlineNew, Abstract)
class UKMAbilityCost : public UObject
{
	GENERATED_BODY()

public:
	UKMAbilityCost()
	{
	}

	virtual bool CheckCost(const UKMCharacterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
	{
		return true;
	}

	virtual void ApplyCost(const UKMCharacterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
	{
	}

	bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Costs)
	bool bOnlyApplyCostOnHit = false;
};

