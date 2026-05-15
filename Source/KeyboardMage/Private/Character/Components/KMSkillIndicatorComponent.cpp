#include "Character/Components/KMSkillIndicatorComponent.h"
#include "Character/KMCharacter.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UKMSkillIndicatorComponent::UKMSkillIndicatorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
    SetIsReplicatedByDefault(false);
}

void UKMSkillIndicatorComponent::TickComponent(
    float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!bIsVisible) return;
    UpdateDecalTransform();
}

// ─────────────────────────────────────────────────────────────────────────────
//  공개 API
// ─────────────────────────────────────────────────────────────────────────────

void UKMSkillIndicatorComponent::ShowIndicator(
    EKMIndicatorShape Shape, float Extent1, float Extent2)
{
    CurrentShape   = Shape;
    CurrentExtent1 = Extent1;
    CurrentExtent2 = Extent2;
    bIsVisible     = true;

    // Circle / Ring 은 에임 포인트 기준
    bFollowAimPoint = (Shape == EKMIndicatorShape::Circle ||
                       Shape == EKMIndicatorShape::Ring);

    CreateDecalIfNeeded();
    if (!DecalComp) return;

    ApplyMaterialParams();

    // 데칼 크기: 가장 큰 범위로 설정
    const float HalfSize = FMath::Max(Extent1, FMath::Abs(Extent2));
    DecalComp->DecalSize  = FVector(DecalHeight, HalfSize, HalfSize);
    DecalComp->SetVisibility(true);

    PrimaryComponentTick.SetTickFunctionEnable(true);
    UpdateDecalTransform();
}

void UKMSkillIndicatorComponent::HideIndicator()
{
    bIsVisible = false;
    if (DecalComp) DecalComp->SetVisibility(false);
    PrimaryComponentTick.SetTickFunctionEnable(false);
}

// ─────────────────────────────────────────────────────────────────────────────
//  머티리얼 파라미터 적용
// ─────────────────────────────────────────────────────────────────────────────

void UKMSkillIndicatorComponent::ApplyMaterialParams()
{
    if (!DecalComp) return;

    UMaterialInterface* Mat = GetMaterialForShape(CurrentShape);
    if (!Mat) return;

    UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(Mat, this);
    if (!MID) return;

    switch (CurrentShape)
    {
    case EKMIndicatorShape::Fan:
        // Extent1 = 반경, Extent2 = 반각(도)
        MID->SetScalarParameterValue(TEXT("Radius"),    CurrentExtent1);
        MID->SetScalarParameterValue(TEXT("HalfAngle"), CurrentExtent2);
        break;

    case EKMIndicatorShape::Rect:
        // Extent1 = 길이(전방), Extent2 = 너비(좌우)
        MID->SetScalarParameterValue(TEXT("Length"), CurrentExtent1);
        MID->SetScalarParameterValue(TEXT("Width"),  CurrentExtent2);
        break;

    case EKMIndicatorShape::Circle:
        // Extent1 = 반경
        MID->SetScalarParameterValue(TEXT("Radius"), CurrentExtent1);
        break;

    case EKMIndicatorShape::Ring:
        // Extent1 = 외부 반경, Extent2 = 내부 반경
        MID->SetScalarParameterValue(TEXT("OuterRadius"), CurrentExtent1);
        MID->SetScalarParameterValue(TEXT("InnerRadius"), CurrentExtent2);
        break;
    }

    DecalComp->SetMaterial(0, MID);
}

UMaterialInterface* UKMSkillIndicatorComponent::GetMaterialForShape(
    EKMIndicatorShape Shape) const
{
    switch (Shape)
    {
    case EKMIndicatorShape::Fan:    return FanMaterial;
    case EKMIndicatorShape::Rect:   return RectMaterial;
    case EKMIndicatorShape::Circle: return CircleMaterial ? CircleMaterial : FanMaterial;
    case EKMIndicatorShape::Ring:   return RingMaterial   ? RingMaterial   : FanMaterial;
    default:                        return FanMaterial;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  내부 구현
// ─────────────────────────────────────────────────────────────────────────────

void UKMSkillIndicatorComponent::CreateDecalIfNeeded()
{
    if (DecalComp) return;

    AActor* Owner = GetOwner();
    if (!Owner) return;

    UWorld* World = Owner->GetWorld();
    if (!World) return;

    DecalComp = NewObject<UDecalComponent>(Owner);
    DecalComp->RegisterComponentWithWorld(World);
    DecalComp->SetWorldRotation(FRotator(-90.f, 0.f, 0.f));
    DecalComp->SetVisibility(false);
}

FVector UKMSkillIndicatorComponent::GetTargetLocation() const
{
    AKMCharacter* KMChar = Cast<AKMCharacter>(GetOwner());
    if (!KMChar) return FVector::ZeroVector;

    if (bFollowAimPoint)
    {
        const FVector AimPt = KMChar->GetCurrentAimPoint();
        if (!AimPt.IsZero()) return AimPt;
    }

    return KMChar->GetActorLocation();
}

void UKMSkillIndicatorComponent::UpdateDecalTransform()
{
    if (!DecalComp) return;

    AKMCharacter* KMChar = Cast<AKMCharacter>(GetOwner());
    if (!KMChar) return;

    const FVector Center = GetTargetLocation();
    FRotator DecalRot    = FRotator(-90.f, 0.f, 0.f);

    switch (CurrentShape)
    {
    case EKMIndicatorShape::Fan:
    {
        // 캐릭터가 바라보는 방향 기준
        const float Yaw = KMChar->GetActorRotation().Yaw;
        DecalRot = FRotator(-90.f, Yaw - 90.f, 0.f);
        break;
    }
    case EKMIndicatorShape::Rect:
    {
        // 에임 포인트 방향 기준
        const FVector AimPt = KMChar->GetCurrentAimPoint();
        if (!AimPt.IsZero())
        {
            const FVector Dir = (AimPt - KMChar->GetActorLocation()).GetSafeNormal2D();
            if (!Dir.IsNearlyZero())
                DecalRot = FRotator(-90.f, Dir.Rotation().Yaw - 90.f, 0.f);
        }
        break;
    }
    case EKMIndicatorShape::Circle:
    case EKMIndicatorShape::Ring:
    {
        // 원형 / 고리형: 항상 수평 (방향 없음)
        DecalRot = FRotator(-90.f, 0.f, 0.f);
        break;
    }
    }

    const FVector DecalPos = Center + FVector(0.f, 0.f, DecalHeight * 0.5f);
    DecalComp->SetWorldLocationAndRotation(DecalPos, DecalRot);
}
