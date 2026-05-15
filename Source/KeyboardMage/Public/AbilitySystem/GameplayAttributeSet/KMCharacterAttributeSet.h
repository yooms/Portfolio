#pragma once
#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "Common/KMSpellratagemTypes.h"
#include "KMCharacterAttributeSet.generated.h"


UENUM(BlueprintType)
enum class EKMAbilityInputID : uint8
{
    None        UMETA(DisplayName = "None"),
    Confirm     UMETA(DisplayName = "Confirm"),
    Cancel      UMETA(DisplayName = "Cancel"),
    Skill1      UMETA(DisplayName = "Skill1"),
    Fire        UMETA(DisplayName = "Fire"),
    Roll       UMETA(DisplayName = "Roll"),
    Aim         UMETA(DisplayName = "Aim"),
    HomingFire  UMETA(DisplayName = "HomingFire"),
    Jump        UMETA(DisplayName = "Jump"),
};


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class KEYBOARDMAGE_API UKMCharacterAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UKMCharacterAttributeSet();

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UKMCharacterAttributeSet, Health)
    UFUNCTION() void OnRep_Health(const FGameplayAttributeData& OldHealth);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UKMCharacterAttributeSet, Mana)
    UFUNCTION() void OnRep_Mana(const FGameplayAttributeData& OldMana);

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina)
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(UKMCharacterAttributeSet, Stamina)
    UFUNCTION() void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
