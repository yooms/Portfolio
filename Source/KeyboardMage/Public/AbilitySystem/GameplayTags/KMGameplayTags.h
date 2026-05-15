#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"


struct KEYBOARDMAGE_API KMGameplayTags
{
    // ── 기존 어빌리티 태그 ────────────────────────────────────────────────
    static FGameplayTag Ability_Fire;           // Character.Projectile.BasicSkill
    static FGameplayTag Ability_Aim;            // Character.Sight
    static FGameplayTag Ability_Jump;           // Character.Motion.Jump
    static FGameplayTag Ability_Roll;           // Character.Motion.Roll
    static FGameplayTag Ability_HomingFire;     // Character.Projectile.Homing

    // ── 모자 스킬 태그 ────────────────────────────────────────────────────
    /** 모자 슬롯 스킬 활성화 공통 태그 (Hat.Skill.Slot0 ~ Hat.Skill.Slot9) */
    static FGameplayTag Ability_HatSkill[10];

    /** 커맨드 입력 모드 진입 태그 */
    static FGameplayTag Ability_CommandInput;   // Hat.CommandInput

    // ── 입력 태그 (GAS AbilitySystemComponent 의 InputID 대신 Tag 사용) ──
    /** 기본 발사 입력 태그 */
    static FGameplayTag Input_Fire;             // Input.Fire
    /** 모자 슬롯 스킬 발사 입력 태그 */
    static FGameplayTag Input_HatFire;          // Input.HatFire
    /** 화살표 Up 입력 태그 */
    static FGameplayTag Input_CommandUp;        // Input.Command.Up
    /** 화살표 Down 입력 태그 */
    static FGameplayTag Input_CommandDown;      // Input.Command.Down
    /** 화살표 Left 입력 태그 */
    static FGameplayTag Input_CommandLeft;      // Input.Command.Left
    /** 화살표 Right 입력 태그 */
    static FGameplayTag Input_CommandRight;     // Input.Command.Right
    /** 숫자 슬롯 선택 입력 태그 (Input.SlotSelect.0 ~ Input.SlotSelect.9) */
    static FGameplayTag Input_SlotSelect[10];

    // ── 상태 태그 ─────────────────────────────────────────────────────────
    static FGameplayTag State_Aiming;           // Character.State.Aiming
    static FGameplayTag State_Rolling;          // Character.State.Rolling
    static FGameplayTag State_Invincible;       // Character.State.Invincible
    static FGameplayTag State_InAir;            // Character.State.InAir
    /** 커맨드 입력 대기 상태 */
    static FGameplayTag State_CommandInputting; // Character.State.CommandInputting

    // ── 블록 태그 ─────────────────────────────────────────────────────────
    static FGameplayTag Block_Movement;         // Character.Block.Movement
    /** 기본 발사 차단 (커맨드 입력 중) */
    static FGameplayTag Block_BasicFire;        // Character.Block.BasicFire

    // ── 이벤트 태그 ───────────────────────────────────────────────────────
    static FGameplayTag Event_Fire;             // Character.Event.Fire
    /** 커맨드 성공 이벤트 */
    static FGameplayTag Event_CommandSuccess;   // Character.Event.CommandSuccess

    static void InitTags();
};
