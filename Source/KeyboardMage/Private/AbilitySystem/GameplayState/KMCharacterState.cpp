#include "AbilitySystem/GameplayState/KMCharacterState.h"

AKMCharacterState::AKMCharacterState()
{
    AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("ASC");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    AttributeSet = CreateDefaultSubobject<UKMCharacterAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AKMCharacterState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}