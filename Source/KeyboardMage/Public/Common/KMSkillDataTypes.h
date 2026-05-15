#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
//#include "Character/Components/KMSkillIndicatorComponent.h"
#include "KMSkillDataTypes.generated.h"

// ─────────────────────────────────────────────────────────────────────────────
//  투사체 발사 패턴
// ─────────────────────────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EKMFirePattern : uint8
{
    Single      UMETA(DisplayName="단발"),           // 단일 투사체
    Spread      UMETA(DisplayName="산탄"),           // 부채꼴 N발 동시 발사
    Burst       UMETA(DisplayName="점사"),           // N발 연속 발사 (딜레이 간격)
    Volley      UMETA(DisplayName="일제사격"),       // 원형 N방향으로 동시 발사
    Line        UMETA(DisplayName="직선관통"),       // 직선으로 N발 나란히
};

// ─────────────────────────────────────────────────────────────────────────────
//  범위 공격 형태
// ─────────────────────────────────────────────────────────────────────────────

UENUM(BlueprintType)
enum class EKMAreaShape : uint8
{
    Fan         UMETA(DisplayName="부채꼴"),
    Rectangle   UMETA(DisplayName="직사각형"),
    Circle      UMETA(DisplayName="원형"),
    Ring        UMETA(DisplayName="고리형"),         // 도넛 모양 (최소~최대 반경)
};

// ─────────────────────────────────────────────────────────────────────────────
//  투사체 스킬 데이터 행 (DataTable 행)
//  에디터: DT_FireSkills
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FKMFireSkillData : public FTableRowBase
{
    GENERATED_BODY()

    /** 스킬 표시 이름 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Info")
    FText DisplayName;

    /** 발사 패턴 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire")
    EKMFirePattern FirePattern = EKMFirePattern::Single;

    /** 발사할 투사체 클래스 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire")
    TSoftClassPtr<class AKMProjectile> ProjectileClass;

    /**
     * 발사 수.
     * Single: 무시 / Spread: 산탄 수 / Burst: 연속 발사 수
     * Volley: 방향 수 / Line: 나란히 발사 수
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire", meta=(ClampMin="1"))
    int32 ProjectileCount = 1;

    /**
     * Spread 총 각도 (도). 산탄 패턴에서만 사용.
     * 예) 60 → 좌우 각 30도로 산탄 분산
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire|Spread", meta=(ClampMin="0.0"))
    float SpreadAngle = 30.f;

    /**
     * Burst 발사 간격 (초). 점사 패턴에서만 사용.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire|Burst", meta=(ClampMin="0.01"))
    float BurstInterval = 0.1f;

    /**
     * Line 발사 간격 (cm). 직선관통 나란히 간격.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire|Line", meta=(ClampMin="10.0"))
    float LineSpacing = 80.f;

    /** 조준 시 탄착군 (도) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire|Accuracy", meta=(ClampMin="0.0"))
    float AimSpreadDeg = 0.5f;

    /** 비조준 탄착군 (도) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fire|Accuracy", meta=(ClampMin="0.0"))
    float HipSpreadDeg = 5.0f;

    /** 쿨다운 (초) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Balance", meta=(ClampMin="0.0"))
    float Cooldown = 1.0f;

    /** 마나 비용 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Balance", meta=(ClampMin="0.0"))
    float ManaCost = 10.f;
};

// ─────────────────────────────────────────────────────────────────────────────
//  범위 스킬 데이터 행 (DataTable 행)
//  에디터: DT_AreaSkills
// ─────────────────────────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FKMAreaFireSkillData : public FTableRowBase
{
    GENERATED_BODY()

    /** 스킬 표시 이름 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Info")
    FText DisplayName;

    /** 범위 형태 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Area")
    EKMAreaShape AreaShape = EKMAreaShape::Fan;

    /**
     * 범위 크기 1.
     * 부채꼴: 반경 / 직사각형: 전방 길이 / 원형: 반경 / 고리: 외부 반경
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Area", meta=(ClampMin="10.0"))
    float Extent1 = 500.f;

    /**
     * 범위 크기 2.
     * 부채꼴: 반각(도) / 직사각형: 너비 / 원형: 무시 / 고리: 내부 반경
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Area", meta=(ClampMin="0.0"))
    float Extent2 = 60.f;

    /** 적용할 GameplayEffect (피해, 버프, 디버프 등) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
    TSoftClassPtr<UGameplayEffect> EffectClass;

    /** GE 레벨 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect", meta=(ClampMin="1"))
    float EffectLevel = 1.f;

    /** 최대 타깃 수 (0 = 무제한) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect", meta=(ClampMin="0"))
    int32 MaxTargets = 0;

    /** 충돌 채널 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
    TEnumAsByte<ECollisionChannel> TargetChannel = ECC_Pawn;

    /** 쿨다운 (초) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Balance", meta=(ClampMin="0.0"))
    float Cooldown = 3.0f;

    /** 마나 비용 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Balance", meta=(ClampMin="0.0"))
    float ManaCost = 30.f;

    /**
     * 인디케이터 머티리얼 오버라이드.
     * 비어있으면 KMSkillIndicatorComponent 의 기본 머티리얼 사용.
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Visual")
    TSoftObjectPtr<UMaterialInterface> IndicatorMaterialOverride;
};
