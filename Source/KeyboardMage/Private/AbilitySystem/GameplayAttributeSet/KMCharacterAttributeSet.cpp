#include "AbilitySystem/GameplayAttributeSet/KMCharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"



UKMCharacterAttributeSet::UKMCharacterAttributeSet()
{
}

void UKMCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UKMCharacterAttributeSet, Health, OldHealth);
}

void UKMCharacterAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UKMCharacterAttributeSet, Mana, OldMana);
}

void UKMCharacterAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UKMCharacterAttributeSet, Stamina, OldStamina);
}

void UKMCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UKMCharacterAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UKMCharacterAttributeSet, Mana, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UKMCharacterAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
}

void UKMCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        float NewHealth = GetHealth();
        SetHealth(FMath::Clamp(NewHealth, 0.f, 100.f));

        UE_LOG(LogTemp, Warning, TEXT("Health Changed: %f"), GetHealth());
    }

    if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        float NewMana = GetMana();
        SetMana(FMath::Clamp(NewMana, 0.f, 100.f));

        UE_LOG(LogTemp, Warning, TEXT("Mana Changed: %f"), GetMana());
    }

    if (Data.EvaluatedData.Attribute == GetManaAttribute())
    {
        float NewStamina = GetStamina();
        SetStamina(FMath::Clamp(NewStamina, 0.f, 100.f));

        UE_LOG(LogTemp, Warning, TEXT("Stamina Changed: %f"), GetStamina());
    }
}