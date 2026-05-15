#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "KMSpellratagemTypes.h"
#include "KMHatTypes.generated.h"

// ─────────────────────────────────────────────────────────────────────────────
//  모자 슬롯 인덱스 (숫자 키 0~9)
//  숫자키 1~9 → 슬롯 0~8, 숫자키 0 → 슬롯 9
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FKMHatSkillInfo
{
    GENERATED_BODY()

    /** 이 모자가 가진 스킬 GA 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hat")
    TSubclassOf<class UGameplayAbility> SkillAbilityClass;

    /** 화살표 커맨드 시퀀스 (UI 표시 + 입력 매칭용) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hat")
    TArray<EKMCommandDirection> CommandSequence;

    /** 모자 표시 이름 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hat")
    FText DisplayName;

    /** 모자 아이콘 (UI용) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hat")
    TSoftObjectPtr<UTexture2D> Icon;

    bool IsValid() const { return SkillAbilityClass != nullptr; }
};

// ─────────────────────────────────────────────────────────────────────────────
//  모자 슬롯 상태
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FKMHatSlot
{
    GENERATED_BODY()

    /** 장착된 모자 정보 */
    UPROPERTY(BlueprintReadOnly, Category="Hat")
    FKMHatSkillInfo HatInfo;

    /** GAS에 등록된 Ability Spec Handle */
    FGameplayAbilitySpecHandle AbilityHandle;

    /** 슬롯이 채워져 있는가 */
    bool IsOccupied() const { return HatInfo.IsValid(); }
};

// ─────────────────────────────────────────────────────────────────────────────
//  입력 상태 — 어떤 IMC가 활성인가
// ─────────────────────────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EKMInputMode : uint8
{
    /** 일반 이동/공격 모드 */
    Normal,
    /** 숫자키 슬롯 선택 후 커맨드 입력 대기 중 */
    CommandInput,
};

// ─────────────────────────────────────────────────────────────────────────────
//  커맨드 입력 결과
// ─────────────────────────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EKMCommandResult : uint8
{
    Pending,    // 입력 중
    Success,    // 커맨드 완성
    Failed,     // 틀린 입력
    Timeout,    // 시간 초과
};

// ─────────────────────────────────────────────────────────────────────────────
//  모자 장착 이벤트 델리게이트
// ─────────────────────────────────────────────────────────────────────────────

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHatEquipped,   int32, SlotIndex, const FKMHatSkillInfo&, HatInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnHatUnequipped, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FOnHatSlotSelected, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCommandInputChanged,
    const TArray<EKMCommandDirection>&, CurrentInput,
    const TArray<EKMCommandDirection>&, TargetSequence);
