#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/KMCharacterGameplayAbility.h"
#include "KMGA_Death.generated.h"



UCLASS()
class KEYBOARDMAGE_API UKMGA_Death : public UKMCharacterGameplayAbility
{
	GENERATED_BODY()
public:
	UKMGA_Death();

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

private:
	void FinishDeath();

};
