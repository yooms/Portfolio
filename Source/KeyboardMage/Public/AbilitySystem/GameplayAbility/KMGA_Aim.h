#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/KMCharacterGameplayAbility.h"
#include "KMGA_Aim.generated.h"



UCLASS()
class KEYBOARDMAGE_API UKMGA_Aim : public UKMCharacterGameplayAbility
{
	GENERATED_BODY()
public:
	UKMGA_Aim();

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
};
