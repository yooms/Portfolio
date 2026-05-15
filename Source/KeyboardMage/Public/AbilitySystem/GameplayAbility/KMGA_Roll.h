#pragma once
#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/KMCharacterGameplayAbility.h"
#include "KMGA_Roll.generated.h"



UCLASS()
class KEYBOARDMAGE_API UKMGA_Roll : public UKMCharacterGameplayAbility
{
	GENERATED_BODY()
public:
	UKMGA_Roll();

	UPROPERTY(EditDefaultsOnly, Category="Roll", meta=(ClampMin="100.0"))
	float RollImpulse = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="Roll", meta=(ClampMin="0.0"))
	float RollZVelocity = 100.f;

	UPROPERTY(EditDefaultsOnly, Category="Roll", meta=(ClampMin="0.1"))
	float RollDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="Roll")
	TSubclassOf<UGameplayEffect> InvincibleEffectClass;

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
	void FinishRoll();

	FTimerHandle RollTimerHandle;
	FActiveGameplayEffectHandle InvincibleEffectHandle;
};
