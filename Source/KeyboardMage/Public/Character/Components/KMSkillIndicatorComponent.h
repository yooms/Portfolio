#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KMSkillIndicatorComponent.generated.h"

/**
 * KMSkillIndicatorComponent (확장)
 *
 * 스킬 범위를 바닥에 표시하는 컴포넌트.
 * 클라이언트 로컬 전용 (서버에서 실행 안 함).
 *
 * 표시 방식: DecalComponent 를 동적 생성하여 바닥에 투영.
 *
 * 지원 형태:
 *   Fan     — 캐릭터 전방 부채꼴   (반경 + 반각도)
 *   Rect    — 직사각형             (길이 + 너비)
 *   Circle  — 원형 (에임포인트 중심) (반경)
 *   Ring    — 도넛형               (외부반경 + 내부반경)
 *
 * 머티리얼 파라미터 이름 규칙:
 *   Fan    머티리얼: Radius, HalfAngle
 *   Rect   머티리얼: Length, Width
 *   Circle 머티리얼: Radius
 *   Ring   머티리얼: OuterRadius, InnerRadius
 */

UENUM(BlueprintType)
enum class EKMIndicatorShape : uint8
{
    Fan     UMETA(DisplayName="부채꼴"),
    Rect    UMETA(DisplayName="직사각형"),
    Circle  UMETA(DisplayName="원형"),
    Ring    UMETA(DisplayName="고리형"),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KEYBOARDMAGE_API UKMSkillIndicatorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UKMSkillIndicatorComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // ── 인디케이터 제어 API ───────────────────────────────────────────────

    /**
     * 인디케이터 표시.
     * @param Shape   형태
     * @param Extent1 Fan:반경 / Rect:길이 / Circle:반경 / Ring:외부반경 (cm)
     * @param Extent2 Fan:반각(도) / Rect:너비 / Circle:무시 / Ring:내부반경 (cm)
     */
    UFUNCTION(BlueprintCallable, Category="Skill")
    void ShowIndicator(EKMIndicatorShape Shape, float Extent1, float Extent2 = 0.f);

    /** 인디케이터 숨김 */
    UFUNCTION(BlueprintCallable, Category="Skill")
    void HideIndicator();

    UFUNCTION(BlueprintPure, Category="Skill")
    bool IsVisible() const { return bIsVisible; }

    // ── 머티리얼 설정 (BP에서 지정) ──────────────────────────────────────

    UPROPERTY(EditAnywhere, Category="Skill|Material")
    UMaterialInterface* FanMaterial;

    UPROPERTY(EditAnywhere, Category="Skill|Material")
    UMaterialInterface* RectMaterial;

    /** 원형 인디케이터 머티리얼 */
    UPROPERTY(EditAnywhere, Category="Skill|Material")
    UMaterialInterface* CircleMaterial;

    /** 고리형 인디케이터 머티리얼 */
    UPROPERTY(EditAnywhere, Category="Skill|Material")
    UMaterialInterface* RingMaterial;

    /** 데칼 투영 높이 */
    UPROPERTY(EditAnywhere, Category="Skill", meta=(ClampMin="50.0"))
    float DecalHeight = 500.f;

    /**
     * true  → 에임 포인트 중심 (Circle / Ring / AOE)
     * false → 캐릭터 위치 중심 (Fan / Rect)
     */
    UPROPERTY(EditAnywhere, Category="Skill")
    bool bFollowAimPoint = false;

private:
    bool              bIsVisible     = false;
    EKMIndicatorShape CurrentShape;
    float             CurrentExtent1 = 0.f;
    float             CurrentExtent2 = 0.f;

    UPROPERTY()
    class UDecalComponent* DecalComp = nullptr;

    void CreateDecalIfNeeded();
    void UpdateDecalTransform();
    void ApplyMaterialParams();
    FVector GetTargetLocation() const;

    UMaterialInterface* GetMaterialForShape(EKMIndicatorShape Shape) const;
};
