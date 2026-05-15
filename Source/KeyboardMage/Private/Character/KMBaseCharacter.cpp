#include "Character/KMBaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/GameplayState/KMCharacterState.h"
#include "AbilitySystem/GameplayTags/KMGameplayTags.h"
#include "AbilitySystem/GameplayAttributeSet/KMCharacterAttributeSet.h"
#include "AbilitySystemComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AKMBaseCharacter::AKMBaseCharacter()
{
    TrajectoryComponent = CreateDefaultSubobject<UCharacterTrajectoryComponent>("CharacterTrajectoryComponent");
    bReplicates = true;
    SetReplicateMovement(true);
    PrimaryActorTick.bCanEverTick = true;
}

UAbilitySystemComponent* AKMBaseCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AKMBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AKMBaseCharacter::InitAbilitySystem()
{
    AKMCharacterState* KMCharacterState = GetPlayerState<AKMCharacterState>();
    if (!KMCharacterState) 
        return;

    AbilitySystemComponent = KMCharacterState->GetAbilitySystemComponent();
    if (!AbilitySystemComponent) 
        return;

    AbilitySystemComponent->InitAbilityActorInfo(KMCharacterState, this);

    if (HasAuthority() && !bAbilitiesGiven)
    {
        GiveDefaultAbilities();
        bAbilitiesGiven = true;
    }
}

void AKMBaseCharacter::GiveDefaultAbilities()
{
    if (!AbilitySystemComponent) 
        return;

    // [수정] C++ StaticClass 를 직접 하드코딩하지 않음.
    // BP_KMCharacter 의 DefaultAbilities 배열에 등록된 BP 클래스를 순회.
    // → BP_GA_FireSpellratagem 에 ProjectileClass 가 설정되어 있으면
    //   그 값이 그대로 유지된 채로 GiveAbility 됨.
    if (DefaultAbilities.Num() == 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[KM] GiveDefaultAbilities: DefaultAbilities 배열이 비어있습니다! "
                 "BP_KMCharacter 의 DefaultAbilities 에 어빌리티 BP 를 등록하세요."));
        return;
    }

    for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultAbilities)
    {
        if (!AbilityClass) 
            continue;

        FGameplayAbilitySpec Spec(AbilityClass, 1);
        AbilitySystemComponent->GiveAbility(Spec);

        UE_LOG(LogTemp, Warning, TEXT("[KM] GiveAbility: %s"), *AbilityClass->GetName());
    }

    UE_LOG(LogTemp, Warning, TEXT("[KM] GiveDefaultAbilities DONE - Total: %d"),
        AbilitySystemComponent->GetActivatableAbilities().Num());
}

void AKMBaseCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    InitAbilitySystem();
}

void AKMBaseCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitAbilitySystem();
}
