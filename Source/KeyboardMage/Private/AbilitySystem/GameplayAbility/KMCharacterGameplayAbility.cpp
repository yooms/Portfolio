#include "AbilitySystem/GameplayAbility/KMCharacterGameplayAbility.h"

UKMCharacterGameplayAbility::UKMCharacterGameplayAbility()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKMCharacterGameplayAbility::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    // 베이스 클래스는 UGameplayAbility 의 Super 만 호출.
    // 여기서 EndAbility 를 호출하면 자식 GA 가 즉시 종료되는 버그가 발생하므로
    // 절대 EndAbility 를 호출하지 않음.
    // 자식 GA 들은 이 클래스의 Super::ActivateAbility 를 호출하지 않아도 되고,
    // 호출해도 안전하게 처리됨.
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
