#include "AbilitySystem/GameplayAbility/KMGA_Roll.h"
#include "Character/KMCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"

UKMGA_Roll::UKMGA_Roll()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UKMGA_Roll::ActivateAbility(
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

    AKMCharacter* KMChar = Cast<AKMCharacter>(ActorInfo->AvatarActor.Get());
    if (!KMChar)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    FVector RollDir = FVector::ZeroVector;
    if (UCharacterMovementComponent* MoveComp = KMChar->GetCharacterMovement())
    {
        const FVector Accel = MoveComp->GetCurrentAcceleration();
        if (!Accel.IsNearlyZero())
            RollDir = Accel.GetSafeNormal2D();
    }
    if (RollDir.IsNearlyZero())
        RollDir = KMChar->GetActorForwardVector();

    const FVector LaunchVelocity = RollDir * RollImpulse + FVector(0.f, 0.f, RollZVelocity);
    KMChar->LaunchCharacter(LaunchVelocity, true, false);

    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
    if (ASC && InvincibleEffectClass)
    {
        FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
        Context.AddSourceObject(this);
        FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(InvincibleEffectClass, 1.f, Context);
        if (Spec.IsValid())
            InvincibleEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }

    if (UWorld* World = KMChar->GetWorld())
    {
        World->GetTimerManager().SetTimer(
            RollTimerHandle, this, &UKMGA_Roll::FinishRoll, RollDuration, false);
    }
}

void UKMGA_Roll::FinishRoll()
{
    EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
               GetCurrentActivationInfo(), true, false);
}

void UKMGA_Roll::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (ActorInfo)
    {
        UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
        if (ASC && InvincibleEffectHandle.IsValid())
            ASC->RemoveActiveGameplayEffect(InvincibleEffectHandle);
    }

    if (ActorInfo && ActorInfo->AvatarActor.IsValid())
    {
        if (UWorld* World = ActorInfo->AvatarActor->GetWorld())
            World->GetTimerManager().ClearTimer(RollTimerHandle);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
