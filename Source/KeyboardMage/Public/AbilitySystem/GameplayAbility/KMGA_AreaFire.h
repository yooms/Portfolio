#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/KMCharacterGameplayAbility.h"
#include "Character/Components/KMSkillIndicatorComponent.h"
#include "Common/KMSkillDataTypes.h"
#include "KMGA_AreaFire.generated.h"

/**
 * KMGA_AreaFire
 *
 * 데이터 테이블 기반 범위 공격 어빌리티.
 *
 * 동작:
 *   LMB Pressed  → ActivateAbility: 인디케이터 표시 (필수)
 *   LMB 홀드 중  → 매 Tick 인디케이터 갱신
 *   LMB Released → 범위 확정 + 범위 내 타깃에 GE 적용
 *
 * 범위 형태 (EKMAreaShape):
 *   Fan       → 캐릭터 전방 부채꼴
 *   Rectangle → 직사각형
 *   Circle    → 원형 (에임 포인트 중심)
 *   Ring      → 도넛형 (내/외부 반경)
 *
 * 설정:
 *   1. DT_AreaSkills 데이터 테이블 생성 (행 구조: FKMAreaFireSkillData)
 *   2. BP_GA_AreaFire → SkillDataTable = DT_AreaSkills
 *   3. BP_GA_AreaFire → SkillRowName = 사용할 행 이름
 */


UCLASS()
class KEYBOARDMAGE_API UKMGA_AreaFire : public UKMCharacterGameplayAbility
{
    GENERATED_BODY()

public:
    UKMGA_AreaFire();

    // ── 데이터 테이블 설정 ────────────────────────────────────────────────

    /** 스킬 데이터 테이블 (FKMAreaFireSkillData 행 구조) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SkillData")
    UDataTable* SkillDataTable;

    /**
     * 현재 사용할 스킬 행 이름.
     * 커맨드에 따라 런타임에 변경 가능.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SkillData")
    FName SkillRowName = NAME_None;

public:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData) override;

    virtual void InputReleased(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled) override;

    /** 런타임 스킬 행 변경 */
    UFUNCTION(BlueprintCallable, Category="SkillData")
    void SetSkillRow(FName NewRowName) { SkillRowName = NewRowName; }

//    UFUNCTION(BlueprintPure, Category="SkillData")
    const FKMAreaFireSkillData* GetCurrentSkillData() const;

private:
    void ShowIndicator(const FGameplayAbilityActorInfo* ActorInfo, const FKMAreaFireSkillData& Data);
    void HideIndicator(const FGameplayAbilityActorInfo* ActorInfo);
    class UKMSkillIndicatorComponent* GetIndicatorComp(const FGameplayAbilityActorInfo* ActorInfo) const;

    void ApplyAreaEffect(const FGameplayAbilityActorInfo* ActorInfo, const FKMAreaFireSkillData& Data);
    TArray<AActor*> GatherTargets(ACharacter* Caster, const FKMAreaFireSkillData& Data) const;

    // 형태별 범위 판정
    bool IsInsideFan      (const FVector& O, const FVector& Fwd, const FVector& T, float R, float HalfDeg) const;
    bool IsInsideRect     (const FVector& O, const FVector& Fwd, const FVector& T, float Len, float HalfW) const;
    bool IsInsideCircle   (const FVector& O, const FVector& T, float R) const;
    bool IsInsideRing     (const FVector& O, const FVector& T, float OuterR, float InnerR) const;

    // 인디케이터 형태 매핑
    EKMIndicatorShape ToIndicatorShape(EKMAreaShape AreaShape) const;
};
