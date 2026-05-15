#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Animation/MotionMatchingComponent.h"
#include "Animation/MotionTrajectoryComponent.h"
#include "CharacterTrajectoryComponent.h"
#include "KMBaseCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(Abstract)
class KEYBOARDMAGE_API AKMBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AKMBaseCharacter();

protected:
    UPROPERTY(EditAnywhere)
    TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCharacterTrajectoryComponent> TrajectoryComponent;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UMotionMatchingComponent> MotionMatchingComponent;

    bool bAbilitiesGiven = false;

public:
    // ── [핵심] BP 캐릭터에서 어빌리티 목록을 직접 설정 ──────────────────
    //
    // 사용법:
    //   BP_KMCharacter (AKMCharacter 의 블루프린트 자식) 열기
    //   → Class Defaults → DefaultAbilities 배열에 추가:
    //       [0] BP_GA_FireSpellratagem   (ProjectileClass 설정된 BP)
    //       [1] BP_GA_Roll
    //       [2] BP_GA_Aim
    //       [3] BP_GA_Jump
    //       [4] BP_GA_HomingFire
    //
    // GiveDefaultAbilities() 가 이 배열을 순회하며 GiveAbility 를 호출.
    // C++ StaticClass 를 직접 등록하지 않으므로 BP 에서 설정한
    // ProjectileClass 등 모든 속성이 올바르게 유지됨.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
    TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UCharacterTrajectoryComponent* GetCharacterTrajectoryComponent() const { return TrajectoryComponent; }

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PossessedBy(AController* NewController) override;
    virtual void OnRep_PlayerState() override;

    void InitAbilitySystem();
    virtual void GiveDefaultAbilities();
};
