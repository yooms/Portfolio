#include "AbilitySystem/GameplayAbility/KMGA_AreaFire.h"
#include "AbilitySystem/GameplayState/KMPlayerState.h"
#include "Character/KMCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "Engine/OverlapInfo.h"
#include "Engine/OverlapResult.h"


UKMGA_AreaFire::UKMGA_AreaFire()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

// ─────────────────────────────────────────────────────────────────────────────
//  데이터 조회
// ─────────────────────────────────────────────────────────────────────────────

const FKMAreaFireSkillData* UKMGA_AreaFire::GetCurrentSkillData() const
{
    if (!SkillDataTable || SkillRowName.IsNone())
        return nullptr;

    return SkillDataTable->FindRow<FKMAreaFireSkillData>(SkillRowName, TEXT("KMGA_AreaFire"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  LMB Pressed → 인디케이터 표시
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_AreaFire::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    const FKMAreaFireSkillData* Data = GetCurrentSkillData();
    if (!Data)
    {
        UE_LOG(LogTemp, Error, TEXT("[KM] KMGA_AreaFire: SkillData 없음 — " "SkillDataTable 과 SkillRowName 을 설정하세요 (Row: %s)"), *SkillRowName.ToString());
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 클라이언트: 인디케이터 즉시 표시
    if (ActorInfo->IsLocallyControlled())
    {
        ShowIndicator(ActorInfo, *Data);
    }

    // EndAbility 호출 안 함 → InputReleased 대기
}

// ─────────────────────────────────────────────────────────────────────────────
//  LMB Released → 범위 피해 적용
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_AreaFire::InputReleased(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    // 클라이언트: 인디케이터 즉시 숨김
    if (ActorInfo->IsLocallyControlled())
    {
        HideIndicator(ActorInfo);
    }

    // 서버: 커맨드 확인 + 범위 피해 적용
    if (ActorInfo->AvatarActor.IsValid() && ActorInfo->AvatarActor->HasAuthority())
    {
        AKMCharacter* KMChar = Cast<AKMCharacter>(ActorInfo->AvatarActor.Get());
        if (KMChar)
        {
            if (AKMPlayerState* PS = KMChar->GetPlayerState<AKMPlayerState>())
            {
                if (!PS->bCommandReady)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[KM] AreaFire: 커맨드 미완성 → 시전 거부"));
                    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
                    return;
                }
                PS->ConsumeAndRequestNew();
            }
        }

        if (const FKMAreaFireSkillData* Data = GetCurrentSkillData())
        {
            ApplyAreaEffect(ActorInfo, *Data);
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UKMGA_AreaFire::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    if (ActorInfo && ActorInfo->IsLocallyControlled())
    {
        HideIndicator(ActorInfo);
    }
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ─────────────────────────────────────────────────────────────────────────────
//  범위 피해 적용
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_AreaFire::ApplyAreaEffect(const FGameplayAbilityActorInfo* ActorInfo, const FKMAreaFireSkillData& Data)
{
    TSubclassOf<UGameplayEffect> EffectClass = Data.EffectClass.LoadSynchronous();
    if (!EffectClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[KM] AreaFire: EffectClass 로드 실패 (Row: %s)"),
            *SkillRowName.ToString());
        return;
    }

    ACharacter* Caster = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
    if (!Caster) return;

    UAbilitySystemComponent* SourceASC = ActorInfo->AbilitySystemComponent.Get();
    if (!SourceASC) return;

    TArray<AActor*> Targets = GatherTargets(Caster, Data);
    UE_LOG(LogTemp, Warning, TEXT("[KM] AreaFire [%s]: %d 명 타깃 (Shape=%d)"),
        *SkillRowName.ToString(), Targets.Num(), (int)Data.AreaShape);

    for (AActor* Target : Targets)
    {
        if (!IsValid(Target)) continue;

        UAbilitySystemComponent* TargetASC =
            UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
        if (!TargetASC) continue;

        FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
        Context.AddSourceObject(Caster);
        Context.AddInstigator(Caster, Caster);

        FGameplayEffectSpecHandle Spec =
            SourceASC->MakeOutgoingSpec(EffectClass, Data.EffectLevel, Context);

        if (Spec.IsValid())
        {
            TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
            UE_LOG(LogTemp, Log, TEXT("[KM] AreaFire: GE 적용 → %s"), *Target->GetName());
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  타깃 수집
// ─────────────────────────────────────────────────────────────────────────────

TArray<AActor*> UKMGA_AreaFire::GatherTargets(ACharacter* Caster, const FKMAreaFireSkillData& Data) const
{
    TArray<AActor*> Result;
    if (!Caster) return Result;

    UWorld* World = Caster->GetWorld();
    if (!World) return Result;

    // Circle/Ring 은 에임 포인트 중심, 나머지는 캐릭터 위치 중심
    FVector Origin = Caster->GetActorLocation();
    if (Data.AreaShape == EKMAreaShape::Circle || Data.AreaShape == EKMAreaShape::Ring)
    {
        if (AKMCharacter* KMChar = Cast<AKMCharacter>(Caster))
        {
            const FVector AimPt = KMChar->GetReplicatedAimPoint();
            if (!AimPt.IsZero()) Origin = AimPt;
        }
    }

    const FVector Forward = Caster->GetActorForwardVector();
    const float CheckRadius = Data.Extent1; // 최대 범위로 Overlap

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Caster);

    World->OverlapMultiByChannel(
        Overlaps, Origin, FQuat::Identity,
        Data.TargetChannel,
        FCollisionShape::MakeSphere(CheckRadius),
        Params);

    for (const FOverlapResult& Overlap : Overlaps)
    {
        AActor* Target = Overlap.GetActor();

        if (!IsValid(Target) || Target == Caster) 
            continue;

        const FVector TargetPos = Target->GetActorLocation();
        bool bInRange = false;

        switch (Data.AreaShape)
        {
        case EKMAreaShape::Fan:
            bInRange = IsInsideFan(Origin, Forward, TargetPos, Data.Extent1, Data.Extent2);
            break;
        case EKMAreaShape::Rectangle:
            bInRange = IsInsideRect(Origin, Forward, TargetPos, Data.Extent1, Data.Extent2 * 0.5f);
            break;
        case EKMAreaShape::Circle:
            bInRange = IsInsideCircle(Origin, TargetPos, Data.Extent1);
            break;
        case EKMAreaShape::Ring:
            bInRange = IsInsideRing(Origin, TargetPos, Data.Extent1, Data.Extent2);
            break;
        }

        if (!bInRange) continue;

        Result.Add(Target);
        if (Data.MaxTargets > 0 && Result.Num() >= Data.MaxTargets) break;
    }

    return Result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  범위 판정
// ─────────────────────────────────────────────────────────────────────────────

bool UKMGA_AreaFire::IsInsideFan(const FVector& O, const FVector& Fwd, const FVector& T, float R, float HalfDeg) const
{
    const FVector D = T - O;
    if (D.Size2D() > R) return false;
    const float Dot = FVector::DotProduct(Fwd.GetSafeNormal2D(), D.GetSafeNormal2D());
    return FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f))) <= HalfDeg;
}

bool UKMGA_AreaFire::IsInsideRect(const FVector& O, const FVector& Fwd, const FVector& T, float Len, float HalfW) const
{
    const FVector D    = T - O;
    const FVector F2D  = Fwd.GetSafeNormal2D();
    const FVector R2D  = FVector(-F2D.Y, F2D.X, 0.f);
    const float LocalX = FVector::DotProduct(D, F2D);
    const float LocalY = FVector::DotProduct(D, R2D);
    return (LocalX >= 0.f && LocalX <= Len) && (FMath::Abs(LocalY) <= HalfW);
}

bool UKMGA_AreaFire::IsInsideCircle(const FVector& O, const FVector& T, float R) const
{
    return (T - O).Size2D() <= R;
}

bool UKMGA_AreaFire::IsInsideRing(const FVector& O, const FVector& T, float OuterR, float InnerR) const
{
    const float Dist = (T - O).Size2D();
    return Dist <= OuterR && Dist >= InnerR;
}

// ─────────────────────────────────────────────────────────────────────────────
//  인디케이터
// ─────────────────────────────────────────────────────────────────────────────

EKMIndicatorShape UKMGA_AreaFire::ToIndicatorShape(EKMAreaShape AreaShape) const
{
    switch (AreaShape)
    {
        case EKMAreaShape::Rectangle: 
            return EKMIndicatorShape::Rect;
        default:
            return EKMIndicatorShape::Fan;
    }
}

UKMSkillIndicatorComponent* UKMGA_AreaFire::GetIndicatorComp(const FGameplayAbilityActorInfo* ActorInfo) const
{
    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid()) 
        return nullptr;

    return ActorInfo->AvatarActor->FindComponentByClass<UKMSkillIndicatorComponent>();
}

void UKMGA_AreaFire::ShowIndicator(const FGameplayAbilityActorInfo* ActorInfo, const FKMAreaFireSkillData& Data)
{
    UKMSkillIndicatorComponent* Indicator = GetIndicatorComp(ActorInfo);
    if (!Indicator)
    {
        UE_LOG(LogTemp, Warning, TEXT("[KM] AreaFire: KMSkillIndicatorComponent 없음"));
        return;
    }

    // 머티리얼 오버라이드가 있으면 적용
    if (!Data.IndicatorMaterialOverride.IsNull())
    {
        UMaterialInterface* Mat = Data.IndicatorMaterialOverride.LoadSynchronous();
        if (Mat)
        {
            const EKMIndicatorShape Shape = ToIndicatorShape(Data.AreaShape);
            if (Shape == EKMIndicatorShape::Fan)
            {
                Indicator->FanMaterial = Mat;
            }
            else
            {
                Indicator->RectMaterial = Mat;
            }
        }
    }

    Indicator->ShowIndicator(ToIndicatorShape(Data.AreaShape), Data.Extent1, Data.Extent2);
}

void UKMGA_AreaFire::HideIndicator(const FGameplayAbilityActorInfo* ActorInfo)
{
    if (UKMSkillIndicatorComponent* Indicator = GetIndicatorComp(ActorInfo))
    {
        Indicator->HideIndicator();
    }
}

