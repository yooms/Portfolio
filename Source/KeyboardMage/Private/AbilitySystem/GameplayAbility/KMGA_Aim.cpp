#include "AbilitySystem/GameplayAbility/KMGA_Aim.h"
#include "Character/KMCharacter.h"

UKMGA_Aim::UKMGA_Aim()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKMGA_Aim::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (AKMCharacter* KMChar = Cast<AKMCharacter>(ActorInfo->AvatarActor.Get()))
        KMChar->Server_SetAim(true);
}

void UKMGA_Aim::InputReleased(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UKMGA_Aim::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (ActorInfo)
    {
        if (AKMCharacter* KMChar = Cast<AKMCharacter>(ActorInfo->AvatarActor.Get()))
            KMChar->Server_SetAim(false);
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
