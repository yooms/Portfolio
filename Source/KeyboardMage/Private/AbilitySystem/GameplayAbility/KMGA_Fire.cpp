#include "AbilitySystem/GameplayAbility/KMGA_Fire.h"
#include "AbilitySystem/GameplayState/KMPlayerState.h"
#include "Character/KMCharacter.h"
#include "Projectile/KMProjectile.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/DataTable.h"

UKMGA_Fire::UKMGA_Fire()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    InstancingPolicy   = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

// ─────────────────────────────────────────────────────────────────────────────
//  데이터 조회
// ─────────────────────────────────────────────────────────────────────────────

const FKMFireSkillData* UKMGA_Fire::GetCurrentSkillData() const
{
    if (!SkillDataTable || SkillRowName.IsNone()) 
        return nullptr;

    return SkillDataTable->FindRow<FKMFireSkillData>(SkillRowName, TEXT("KMGA_Fire"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  LMB Pressed → ActivateAbility
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_Fire::ActivateAbility(
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

    // 데이터 유효성 확인
    if (!GetCurrentSkillData())
    {
        UE_LOG(LogTemp, Error, TEXT("[KM] KMGA_Fire: SkillData 없음 — "
            "SkillDataTable 과 SkillRowName 을 설정하세요 (Row: %s)"),
            *SkillRowName.ToString());
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    // 인디케이터 표시 (설정된 경우)
    if (bShowIndicator && ActorInfo->IsLocallyControlled())
    {
        if (auto* Indicator = ActorInfo->AvatarActor->FindComponentByClass<UKMSkillIndicatorComponent>())
        {
            const FKMFireSkillData* Data = GetCurrentSkillData();
            // Fire 스킬은 단순 직선 인디케이터로 표시
            Indicator->ShowIndicator(EKMIndicatorShape::Fan, 1000.f, 10.f);
        }
    }

    // 즉시 발사형(홀드 불필요)일 경우 바로 발사하고 종료
    // 홀드가 필요하면 여기서 EndAbility 를 호출하지 않고 InputReleased 를 대기
    // 기본값: InputReleased 에서 발사
}

// ─────────────────────────────────────────────────────────────────────────────
//  LMB Released → InputReleased → 발사
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_Fire::InputReleased(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo)
{
    // 인디케이터 숨김
    if (ActorInfo->IsLocallyControlled())
    {
        if (auto* Indicator = ActorInfo->AvatarActor->FindComponentByClass<UKMSkillIndicatorComponent>())
            Indicator->HideIndicator();
    }

    // 서버에서 발사 처리
    if (ActorInfo->AvatarActor.IsValid() && ActorInfo->AvatarActor->HasAuthority())
    {
        AKMCharacter* KMChar = Cast<AKMCharacter>(ActorInfo->AvatarActor.Get());

        // 커맨드 게이트
        if (KMChar)
        {
            if (AKMPlayerState* PS = KMChar->GetPlayerState<AKMPlayerState>())
            {
/*
                if (!PS->bCommandReady)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[KM] KMGA_Fire: 커맨드 미완성 → 발사 거부"));
                    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
                    return;
                }
*/
                PS->ConsumeAndRequestNew();
            }
        }

        if (const FKMFireSkillData* Data = GetCurrentSkillData())
        {
            ExecuteFire(ActorInfo, *Data);
        }
    }

    // Burst 는 타이머로 비동기 진행 → EndAbility 를 Burst 완료 후 호출
    // 나머지 패턴은 즉시 종료
    const FKMFireSkillData* Data = GetCurrentSkillData();
    if (!Data || Data->FirePattern != EKMFirePattern::Burst)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }
}

void UKMGA_Fire::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled)
{
    // 인디케이터 정리
    if (ActorInfo && ActorInfo->IsLocallyControlled())
    {
        if (auto* Indicator = ActorInfo->AvatarActor.IsValid()
            ? ActorInfo->AvatarActor->FindComponentByClass<UKMSkillIndicatorComponent>()
            : nullptr)
        {
            Indicator->HideIndicator();
        }
    }

    // Burst 타이머 정리
    if (ActorInfo && ActorInfo->AvatarActor.IsValid())
    {
        if (UWorld* World = ActorInfo->AvatarActor->GetWorld())
            World->GetTimerManager().ClearTimer(BurstTimerHandle);
    }
    PendingBurst.Reset();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

// ─────────────────────────────────────────────────────────────────────────────
//  패턴 분기
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_Fire::ExecuteFire(const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data)
{
    AKMCharacter* KMChar = Cast<AKMCharacter>(ActorInfo->AvatarActor.Get());
    if (!KMChar) 
        return;

    // 투사체 클래스 로드 (SoftClassPtr → 하드 레퍼런스)
    TSubclassOf<AKMProjectile> ProjClass = Data.ProjectileClass.LoadSynchronous();
    if (!ProjClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[KM] KMGA_Fire: ProjectileClass 로드 실패 (Row: %s)"), *SkillRowName.ToString());
        return;
    }

    const FVector MuzzleLoc = GetMuzzleLocation(KMChar);
    const FVector AimDir    = GetAimDirection(KMChar, MuzzleLoc);

    UE_LOG(LogTemp, Log, TEXT("[KM] KMGA_Fire: Pattern=%d, Count=%d, Row=%s"), (int)Data.FirePattern, Data.ProjectileCount, *SkillRowName.ToString());

    switch (Data.FirePattern)
    {
    case EKMFirePattern::Single:  FireSingle (ActorInfo, Data, MuzzleLoc, AimDir); break;
    case EKMFirePattern::Spread:  FireSpread (ActorInfo, Data, MuzzleLoc, AimDir); break;
    case EKMFirePattern::Burst:   FireBurst  (ActorInfo, Data, MuzzleLoc, AimDir); break;
    case EKMFirePattern::Volley:  FireVolley (ActorInfo, Data, MuzzleLoc, AimDir); break;
    case EKMFirePattern::Line:    FireLine   (ActorInfo, Data, MuzzleLoc, AimDir); break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Single — 에임 방향으로 투사체 1발
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_Fire::FireSingle(const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir)
{
    TSubclassOf<AKMProjectile> ProjClass = Data.ProjectileClass.LoadSynchronous();
    AKMCharacter* KMChar = Cast<AKMCharacter>(ActorInfo->AvatarActor.Get());
    const float SpreadDeg = KMChar && KMChar->IsAim() ? Data.AimSpreadDeg : Data.HipSpreadDeg;
    const FVector Dir = ApplySpread(AimDir, SpreadDeg);
    SpawnOne(ActorInfo, ProjClass, MuzzleLoc, Dir.Rotation());
}

// ─────────────────────────────────────────────────────────────────────────────
//  Spread — 부채꼴로 N발 동시 발사
//  SpreadAngle 전체 각도를 N-1 등분
//  예) Count=3, SpreadAngle=60 → -30, 0, +30 도
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_Fire::FireSpread(const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir)
{
    TSubclassOf<AKMProjectile> ProjClass = Data.ProjectileClass.LoadSynchronous();
    const int32 Count = FMath::Max(1, Data.ProjectileCount);

    // AimDir 에 수직인 Up 벡터 구성
    FVector Up, Right;
    AimDir.FindBestAxisVectors(Up, Right);

    for (int32 i = 0; i < Count; i++)
    {
        float AngleDeg = 0.f;
        if (Count > 1)
        {
            AngleDeg = FMath::Lerp(-Data.SpreadAngle * 0.5f,
                                    Data.SpreadAngle * 0.5f,
                                    (float)i / (float)(Count - 1));
        }

        const FVector Dir = AimDir.RotateAngleAxis(AngleDeg, Up);
        SpawnOne(ActorInfo, ProjClass, MuzzleLoc, Dir.Rotation());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Burst — N발을 BurstInterval 간격으로 순차 발사
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_Fire::FireBurst(const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir)
{
    TSubclassOf<AKMProjectile> ProjClass = Data.ProjectileClass.LoadSynchronous();
    AKMCharacter* KMChar = Cast<AKMCharacter>(ActorInfo->AvatarActor.Get());
    const float SpreadDeg = KMChar && KMChar->IsAim() ? Data.AimSpreadDeg : Data.HipSpreadDeg;

    // 첫 발 즉시 발사
    const FVector FirstDir = ApplySpread(AimDir, SpreadDeg);
    SpawnOne(ActorInfo, ProjClass, MuzzleLoc, FirstDir.Rotation());

    const int32 Remaining = Data.ProjectileCount - 1;
    if (Remaining <= 0) 
        return;

    // 남은 발 타이머로 처리
    PendingBurst = FBurstState{
        MuzzleLoc, AimDir, ProjClass, SpreadDeg, Remaining
    };

    if (UWorld* World = KMChar->GetWorld())
    {
        World->GetTimerManager().SetTimer(
            BurstTimerHandle, this,
            &UKMGA_Fire::OnBurstTick,
            Data.BurstInterval, true);
    }
}

void UKMGA_Fire::OnBurstTick()
{
    if (!PendingBurst.IsSet()) 
        return;

    FBurstState& State = PendingBurst.GetValue();
    const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo();

    if (!ActorInfo) 
        return;

    const FVector Dir = ApplySpread(State.AimDir, State.SpreadDeg);
    SpawnOne(ActorInfo, State.ProjClass, State.MuzzleLoc, Dir.Rotation());

    State.Remaining--;
    if (State.Remaining <= 0)
    {
        // Burst 완료 → 어빌리티 종료
        if (ActorInfo->AvatarActor.IsValid())
        {
            if (UWorld* World = ActorInfo->AvatarActor->GetWorld())
                World->GetTimerManager().ClearTimer(BurstTimerHandle);
        }
        PendingBurst.Reset();

        EndAbility(GetCurrentAbilitySpecHandle(), ActorInfo,
                   GetCurrentActivationInfo(), true, false);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Volley — 360도 방사형 N발 동시 발사
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_Fire::FireVolley(const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir)
{
    TSubclassOf<AKMProjectile> ProjClass = Data.ProjectileClass.LoadSynchronous();
    const int32 Count = FMath::Max(1, Data.ProjectileCount);
    const float AngleStep = 360.f / Count;

    for (int32 i = 0; i < Count; i++)
    {
        const float Yaw = AimDir.Rotation().Yaw + AngleStep * i;
        const FRotator Rot(0.f, Yaw, 0.f);
        const FVector Dir = Rot.Vector();
        SpawnOne(ActorInfo, ProjClass, MuzzleLoc, Dir.Rotation());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Line — 에임 방향으로 나란히 N발 (좌우 LineSpacing 간격)
// ─────────────────────────────────────────────────────────────────────────────

void UKMGA_Fire::FireLine(const FGameplayAbilityActorInfo* ActorInfo, const FKMFireSkillData& Data, const FVector& MuzzleLoc, const FVector& AimDir)
{
    TSubclassOf<AKMProjectile> ProjClass = Data.ProjectileClass.LoadSynchronous();
    const int32 Count = FMath::Max(1, Data.ProjectileCount);

    // AimDir 에 수직인 Right 벡터
    FVector Up, Right;
    AimDir.FindBestAxisVectors(Up, Right);

    const float TotalWidth = Data.LineSpacing * (Count - 1);
    const FVector StartOffset = Right * (-TotalWidth * 0.5f);

    for (int32 i = 0; i < Count; i++)
    {
        const FVector SpawnLoc = MuzzleLoc + StartOffset + Right * (Data.LineSpacing * i);
        SpawnOne(ActorInfo, ProjClass, SpawnLoc, AimDir.Rotation());
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  공통 헬퍼
// ─────────────────────────────────────────────────────────────────────────────

AKMProjectile* UKMGA_Fire::SpawnOne(const FGameplayAbilityActorInfo* ActorInfo, TSubclassOf<AKMProjectile> ProjClass, const FVector& Location, const FRotator& Rotation)
{
    if (!ProjClass) 
        return nullptr;

    AActor* Avatar = ActorInfo->AvatarActor.Get();
    if (!Avatar) 
        return nullptr;

    UWorld* World = Avatar->GetWorld();
    if (!World) 
        return nullptr;

    FActorSpawnParameters Params;
    Params.Owner      = Avatar;
    Params.Instigator = Cast<APawn>(Avatar);
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AKMProjectile* Proj = World->SpawnActor<AKMProjectile>(ProjClass, Location, Rotation, Params);
    if (Proj)
    {
        Proj->SetSourceASC(GetAbilitySystemComponentFromActorInfo());
    }
    return Proj;
}

FVector UKMGA_Fire::GetMuzzleLocation(AKMCharacter* KMChar) const
{
    if (KMChar->GetMesh() && KMChar->GetMesh()->DoesSocketExist(TEXT("Muzzle")))
        return KMChar->GetMesh()->GetSocketLocation(TEXT("Muzzle"));

    return KMChar->GetActorLocation() + KMChar->GetActorForwardVector() * 100.f;
}

FVector UKMGA_Fire::GetAimDirection(AKMCharacter* KMChar, const FVector& MuzzleLoc) const
{
    FVector AimPt = KMChar->GetReplicatedAimPoint();
    if (AimPt.IsZero())
        AimPt = MuzzleLoc + KMChar->GetActorForwardVector() * 15000.f;

    FVector Dir = (AimPt - MuzzleLoc).GetSafeNormal();
    return Dir.IsNearlyZero() ? KMChar->GetActorForwardVector() : Dir;
}

FVector UKMGA_Fire::ApplySpread(const FVector& BaseDir, float SpreadDeg) const
{
    if (SpreadDeg <= KINDA_SMALL_NUMBER) 
        return BaseDir;

    const float HalfAngleRad = FMath::DegreesToRadians(SpreadDeg);
    const float CosAngle     = FMath::RandRange(FMath::Cos(HalfAngleRad), 1.f);
    const float SinAngle     = FMath::Sqrt(1.f - CosAngle * CosAngle);
    const float RandomPhi    = FMath::RandRange(0.f, 2.f * PI);

    FVector Up, Right;
    BaseDir.FindBestAxisVectors(Up, Right);

    return (BaseDir * CosAngle +
            Right   * (SinAngle * FMath::Cos(RandomPhi)) +
            Up      * (SinAngle * FMath::Sin(RandomPhi))).GetSafeNormal();
}
