#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/KMCharacterGameplayAbility.h"
#include "Character/Components/KMSkillIndicatorComponent.h"
#include "Common/KMSkillDataTypes.h"
#include "KMGA_Fire.generated.h"

/**
 * KMGA_Fire
 *
 * 데이터 테이블 기반 투사체 발사 어빌리티.
 *
 * 동작:
 *   LMB Pressed  → ActivateAbility: 인디케이터 표시 (선택), 홀드 대기
 *   LMB Released → InputReleased: 발사 패턴에 따라 투사체 스폰
 *
 * 발사 패턴 (EKMFirePattern):
 *   Single  → 단일 투사체 1발
 *   Spread  → 부채꼴로 N발 동시
 *   Burst   → N발 시간 간격으로 연속
 *   Volley  → 360도 방사형 N발
 *   Line    → 좌우로 나란히 N발
 *
 * 설정:
 *   1. DT_FireSkills 데이터 테이블 생성 (행 구조: FKMFireSkillData)
 *   2. BP_GA_Fire → SkillDataTable = DT_FireSkills
 *   3. BP_GA_Fire → SkillRowName = 사용할 행 이름 (런타임 변경 가능)
 */


UCLASS()
class KEYBOARDMAGE_API UKMGA_Fire : public UKMCharacterGameplayAbility
{
    GENERATED_BODY()

public:
    UKMGA_Fire();

    // ── 데이터 테이블 설정 ────────────────────────────────────────────────

    /** 스킬 데이터 테이블 (FKMFireSkillData 행 구조) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SkillData")
    UDataTable* SkillDataTable;

    /**
     * 현재 사용할 스킬 행 이름.
     * 런타임에 변경하면 다른 스킬로 즉시 전환 가능.
     * 예) 커맨드에 따라 다른 SkillRowName 설정
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="SkillData")
    FName SkillRowName = NAME_None;

    // ── 인디케이터 표시 여부 (투사체 스킬은 선택적) ─────────────────────

    UPROPERTY(EditDefaultsOnly, Category="Indicator")
    bool bShowIndicator = false;

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

    /** 런타임에 스킬 행 변경 (커맨드별 스킬 전환용) */
    UFUNCTION(BlueprintCallable, Category="SkillData")
    void SetSkillRow(FName NewRowName) { SkillRowName = NewRowName; }

    /** 현재 스킬 데이터 조회 */
//    UFUNCTION(BlueprintPure, Category="SkillData")
    const FKMFireSkillData* GetCurrentSkillData() const;

private:
    void ExecuteFire(const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data);

    // 패턴별 발사 함수
    void FireSingle (const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir);
    void FireSpread (const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir);
    void FireBurst  (const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir);
    void FireVolley (const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir);
    void FireLine   (const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir);

    // 단일 투사체 스폰 헬퍼
    AKMProjectile* SpawnOne(const FGameplayAbilityActorInfo* ActorInfo,
                             TSubclassOf<AKMProjectile> ProjClass,
                             const FVector& Location, const FRotator& Rotation);

    FVector GetMuzzleLocation(AKMCharacter* KMChar) const;
    FVector GetAimDirection(AKMCharacter* KMChar, const FVector& MuzzleLoc) const;
    FVector ApplySpread(const FVector& BaseDir, float SpreadDeg) const;

    // Burst 타이머 상태
    struct FBurstState
    {
        FVector MuzzleLoc;
        FVector AimDir;
        TSubclassOf<AKMProjectile> ProjClass;
        float SpreadDeg;
        int32 Remaining;
    };
    TOptional<FBurstState> PendingBurst;
    FTimerHandle BurstTimerHandle;
    UFUNCTION() void OnBurstTick();
};
