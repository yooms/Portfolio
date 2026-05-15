#include "Character/Components/KMInputHandlerComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Character/KMCharacter.h"
#include "Character/Components/KMHatComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GameplayAbility/KMGA_Fire.h"
#include "AbilitySystem/GameplayAbility/KMGA_Jump.h"
#include "AbilitySystem/GameplayAbility/KMGA_Roll.h"
#include "AbilitySystem/GameplayAbility/KMGA_Aim.h"
#include "AbilitySystem/GameplayAbility/KMGA_HomingFire.h"
#include "AbilitySystem/GameplayTags/KMGameplayTags.h"
#include "GameFramework/PlayerController.h"
#include "KeyboardMage/KeyboardMage.h"




// ─────────────────────────────────────────────────────────────────────────────
//  초기화
// ─────────────────────────────────────────────────────────────────────────────

void UKMInputHandlerComponent::Initialize(UEnhancedInputComponent* InputComp, AKMCharacter* InOwner)
{
    OwnerCharacter = InOwner;
    if (!InputComp || !OwnerCharacter) return;

    // ── IMC 등록 ──────────────────────────────────────────────────────────
    RegisterIMCs();

    // ── [1] 이동 IMC 바인딩 ───────────────────────────────────────────────
    if (MoveAction)
        InputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &UKMInputHandlerComponent::Move);

    if (LookAction)
        InputComp->BindAction(LookAction, ETriggerEvent::Triggered, this, &UKMInputHandlerComponent::Look);

    if (MouseLookAction)
        InputComp->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &UKMInputHandlerComponent::Look);

    if (JumpAction)
    {
        InputComp->BindAction(JumpAction, ETriggerEvent::Started,   this, &UKMInputHandlerComponent::JumpStart);
        InputComp->BindAction(JumpAction, ETriggerEvent::Completed, this, &UKMInputHandlerComponent::JumpEnd);
    }

    if (AimAction)
    {
        InputComp->BindAction(AimAction, ETriggerEvent::Started,   this, &UKMInputHandlerComponent::AimPressed);
        InputComp->BindAction(AimAction, ETriggerEvent::Completed, this, &UKMInputHandlerComponent::AimReleased);
    }

    if (RollAction)
        InputComp->BindAction(RollAction, ETriggerEvent::Started, this, &UKMInputHandlerComponent::Roll);

    if (HomingFireAction)
        InputComp->BindAction(HomingFireAction, ETriggerEvent::Started, this, &UKMInputHandlerComponent::HomingFire);

    // LMB — Started(Pressed) / Completed(Released)
    if (FireAction)
    {
        InputComp->BindAction(FireAction, ETriggerEvent::Started,   this, &UKMInputHandlerComponent::FirePressed);
        InputComp->BindAction(FireAction, ETriggerEvent::Completed, this, &UKMInputHandlerComponent::FireReleased);
    }

    // ── [2] 커맨드 IMC 바인딩 ─────────────────────────────────────────────
    if (CommandUpAction)
        InputComp->BindAction(CommandUpAction,    ETriggerEvent::Started, this, &UKMInputHandlerComponent::CommandUp);
    if (CommandDownAction)
        InputComp->BindAction(CommandDownAction,  ETriggerEvent::Started, this, &UKMInputHandlerComponent::CommandDown);
    if (CommandLeftAction)
        InputComp->BindAction(CommandLeftAction,  ETriggerEvent::Started, this, &UKMInputHandlerComponent::CommandLeft);
    if (CommandRightAction)
        InputComp->BindAction(CommandRightAction, ETriggerEvent::Started, this, &UKMInputHandlerComponent::CommandRight);

/*
    for (int32 i = 0; i < SlotSelectActions.Num(); i++)
    {
        if (!SlotSelectActions[i]) continue;

        switch (i)
        {
        case 0:
            InputComp->BindAction(
                SlotSelectActions[i],
                ETriggerEvent::Started,
                this,
                &UKMInputHandlerComponent::InputSlot1);
            break;

        case 1:
            InputComp->BindAction(
                SlotSelectActions[i],
                ETriggerEvent::Started,
                this,
                &UKMInputHandlerComponent::InputSlot2);
            break;

        case 2:
            InputComp->BindAction(
                SlotSelectActions[i],
                ETriggerEvent::Started,
                this,
                &UKMInputHandlerComponent::InputSlot3);
            break;
        }
    }
*/

    using FSlotFunc =
        void (UKMInputHandlerComponent::*)(
            const FInputActionValue&);


    FSlotFunc SlotFuncs[10] =
    {
        &UKMInputHandlerComponent::InputSlot0,
        &UKMInputHandlerComponent::InputSlot1,
        &UKMInputHandlerComponent::InputSlot2,
        &UKMInputHandlerComponent::InputSlot3,
        &UKMInputHandlerComponent::InputSlot4,
        &UKMInputHandlerComponent::InputSlot5,
        &UKMInputHandlerComponent::InputSlot6,
        &UKMInputHandlerComponent::InputSlot7,
        &UKMInputHandlerComponent::InputSlot8,
        &UKMInputHandlerComponent::InputSlot9
    };

    for (int32 i = 0; i < SlotSelectActions.Num() && i < 10; i++)
    {
        if (!SlotSelectActions[i]) continue;

        InputComp->BindAction(
            SlotSelectActions[i],
            ETriggerEvent::Started,
            this,
            SlotFuncs[i]);
    }

/*
    // ── [3] 슬롯 선택 IMC 바인딩 ─────────────────────────────────────────
    // SlotSelectActions[0]=키1→슬롯0, ..., SlotSelectActions[8]=키9→슬롯8, SlotSelectActions[9]=키0→슬롯9
    for (int32 i = 0; i < SlotSelectActions.Num() && i < 10; i++)
    {
        if (!SlotSelectActions[i]) continue;

        // 람다 캡처: 슬롯 인덱스
        const int32 CapturedSlot = i;
        InputComp->BindAction(SlotSelectActions[i], ETriggerEvent::Started, this
            [CapturedSlot](const FInputActionValue&)
            {
                OnSlotSelect(CapturedSlot);
            });
    }
*/
}

void UKMInputHandlerComponent::RegisterIMCs()
{
    if (!OwnerCharacter) return;

    APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PC) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
    if (!Subsystem) return;

    // 모든 IMC 클리어 후 재등록 (중복 방지)
    if (IMC_Movement)
        Subsystem->AddMappingContext(IMC_Movement,   0);  // Priority 0 — 이동 (기본)

    if (IMC_Command)
        Subsystem->AddMappingContext(IMC_Command,    1);  // Priority 1 — 커맨드 화살표

    if (IMC_SlotSelect)
        Subsystem->AddMappingContext(IMC_SlotSelect, 2);  // Priority 2 — 숫자키 슬롯

    UE_LOG(LogKeyboardMage, Log,
        TEXT("[InputHandler] IMCs registered — Movement:%s, Command:%s, SlotSelect:%s"),
        IMC_Movement   ? TEXT("OK") : TEXT("NULL"),
        IMC_Command    ? TEXT("OK") : TEXT("NULL"),
        IMC_SlotSelect ? TEXT("OK") : TEXT("NULL"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  [1] 이동 IMC 핸들러
// ─────────────────────────────────────────────────────────────────────────────

void UKMInputHandlerComponent::JumpStart()
{
    TryActivateAbilityByClass(UKMGA_Jump::StaticClass());
}

void UKMInputHandlerComponent::JumpEnd()
{
    SendAbilityInputReleased(UKMGA_Jump::StaticClass());
}

void UKMInputHandlerComponent::Move(const FInputActionValue& Value)
{
    if (!OwnerCharacter) return;
    FVector2D V = Value.Get<FVector2D>();
    OwnerCharacter->DoMove(V.X, V.Y);
}

void UKMInputHandlerComponent::Look(const FInputActionValue& Value)
{
    if (!OwnerCharacter) return;
    FVector2D V = Value.Get<FVector2D>();
    OwnerCharacter->DoLook(V.X, V.Y);
}

void UKMInputHandlerComponent::AimPressed()
{
    TryActivateAbilityByClass(UKMGA_Aim::StaticClass());
}

void UKMInputHandlerComponent::AimReleased()
{
    SendAbilityInputReleased(UKMGA_Aim::StaticClass());
}

void UKMInputHandlerComponent::Roll()
{
    TryActivateAbilityByClass(UKMGA_Roll::StaticClass());
}

void UKMInputHandlerComponent::HomingFire()
{
    TryActivateAbilityByClass(UKMGA_HomingFire::StaticClass());
}

// ─────────────────────────────────────────────────────────────────────────────
//  LMB — 기본 발사 vs 모자 스킬 발사 분기
// ─────────────────────────────────────────────────────────────────────────────

void UKMInputHandlerComponent::FirePressed()
{
    if (!OwnerCharacter) return;

    UKMHatComponent* HatComp = OwnerCharacter->FindComponentByClass<UKMHatComponent>();

    // 모자 슬롯이 선택되어 있고 커맨드가 완성된 경우
    if (HatComp && HatComp->GetSelectedSlotIndex() >= 0 && HatComp->IsCommandReady())
    {
        // 모자 스킬 GA 활성화 시도
        // (커맨드 완성 확인은 HatComponent에서 이미 완료됨)
        const bool bActivated = HatComp->ActivateSelectedSlotSkill();
        if (bActivated)
        {
            UE_LOG(LogKeyboardMage, Log,
                TEXT("[InputHandler] Hat skill activated for slot %d"),
                HatComp->GetSelectedSlotIndex());
            return;
        }
    }

    // 기본 발사 (약한 원거리 공격)
    TryActivateAbilityByClass(UKMGA_Fire::StaticClass());
}

void UKMInputHandlerComponent::FireReleased()
{
    // KMGA_Fire가 홀드-릴리즈 방식일 경우 InputReleased 전달
    SendAbilityInputReleased(UKMGA_Fire::StaticClass());
}

// ─────────────────────────────────────────────────────────────────────────────
//  [2] 커맨드 IMC 핸들러 — HatComponent 로 전달
// ─────────────────────────────────────────────────────────────────────────────

void UKMInputHandlerComponent::CommandUp(const FInputActionValue& Value)
{
    if (!OwnerCharacter) return;
    UKMHatComponent* HatComp = OwnerCharacter->FindComponentByClass<UKMHatComponent>();
    if (!HatComp) return;

    const bool bOk = HatComp->AddCommandInput(EKMCommandDirection::Up);
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ↑ %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::CommandDown(const FInputActionValue& Value)
{
    if (!OwnerCharacter) return;
    UKMHatComponent* HatComp = OwnerCharacter->FindComponentByClass<UKMHatComponent>();
    if (!HatComp) return;

    const bool bOk = HatComp->AddCommandInput(EKMCommandDirection::Down);
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ↓ %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::CommandLeft(const FInputActionValue& Value)
{
    if (!OwnerCharacter) return;
    UKMHatComponent* HatComp = OwnerCharacter->FindComponentByClass<UKMHatComponent>();
    if (!HatComp) return;

    const bool bOk = HatComp->AddCommandInput(EKMCommandDirection::Left);
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ← %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::CommandRight(const FInputActionValue& Value)
{
    if (!OwnerCharacter) return;
    UKMHatComponent* HatComp = OwnerCharacter->FindComponentByClass<UKMHatComponent>();
    if (!HatComp) return;

    const bool bOk = HatComp->AddCommandInput(EKMCommandDirection::Right);
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] → %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

// ─────────────────────────────────────────────────────────────────────────────
//  [3] 슬롯 선택 핸들러
// ─────────────────────────────────────────────────────────────────────────────

void UKMInputHandlerComponent::OnSlotSelect(int32 SlotIndex)
{
    if (!OwnerCharacter) return;

    UKMHatComponent* HatComp = OwnerCharacter->FindComponentByClass<UKMHatComponent>();
    if (!HatComp)
    {
        UE_LOG(LogKeyboardMage, Warning,
            TEXT("[InputHandler] SlotSelect: KMHatComponent not found"));
        return;
    }

    HatComp->SelectSlot(SlotIndex);

    // 선택된 모자 정보 로그
    if (HatComp->IsSlotOccupied(SlotIndex))
    {
        const FKMHatSkillInfo& Info = HatComp->GetHatInfo(SlotIndex);
        FString CmdStr;
        for (EKMCommandDirection D : Info.CommandSequence)
        {
            switch (D)
            {
            case EKMCommandDirection::Up:    CmdStr += TEXT("↑"); break;
            case EKMCommandDirection::Down:  CmdStr += TEXT("↓"); break;
            case EKMCommandDirection::Left:  CmdStr += TEXT("←"); break;
            case EKMCommandDirection::Right: CmdStr += TEXT("→"); break;
            }
        }
        UE_LOG(LogKeyboardMage, Warning,
            TEXT("[InputHandler] Slot %d selected — Hat: '%s', Command: [%s]"),
            SlotIndex, *Info.DisplayName.ToString(), *CmdStr);
    }
    else
    {
        UE_LOG(LogKeyboardMage, Log,
            TEXT("[InputHandler] Slot %d selected — empty"), SlotIndex);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  GAS 헬퍼
// ─────────────────────────────────────────────────────────────────────────────

void UKMInputHandlerComponent::TryActivateAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass)
{
    if (!IsValid(OwnerCharacter)) return;
    UAbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent();
    if (!ASC) return;

    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.Ability && Spec.Ability->GetClass()->IsChildOf(AbilityClass))
        {
            ASC->TryActivateAbility(Spec.Handle);
            return;
        }
    }

    UE_LOG(LogKeyboardMage, Warning,
        TEXT("[InputHandler] TryActivateAbilityByClass: '%s' not found"),
        *AbilityClass->GetName());
}

void UKMInputHandlerComponent::SendAbilityInputReleased(TSubclassOf<UGameplayAbility> AbilityClass)
{
    if (!IsValid(OwnerCharacter)) return;
    UAbilitySystemComponent* ASC = OwnerCharacter->GetAbilitySystemComponent();
    if (!ASC) return;

    for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.Ability && Spec.Ability->GetClass()->IsChildOf(AbilityClass) && Spec.IsActive())
        {
            ASC->AbilitySpecInputReleased(Spec);
            return;
        }
    }
}

void UKMInputHandlerComponent::InputSlot0(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ↑ %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot1(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ↓ %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot2(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ← %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot3(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ↑ %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot4(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ↓ %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot5(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ← %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot6(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ↑ %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot7(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ↓ %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot8(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ← %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}

void UKMInputHandlerComponent::InputSlot9(const FInputActionValue& Value)
{
    bool bOk = true;
    UE_LOG(LogKeyboardMage, Verbose, TEXT("[Cmd] ← %s"), bOk ? TEXT("OK") : TEXT("WRONG"));
}
