#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GameplayAttributeSet/KMCharacterAttributeSet.h"
#include "KMCharacterState.generated.h"

UCLASS()
class KEYBOARDMAGE_API AKMCharacterState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AKMCharacterState();

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UKMCharacterAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
    UPROPERTY()
    UAbilitySystemComponent* AbilitySystemComponent;

    UPROPERTY()
    UKMCharacterAttributeSet* AttributeSet;
};