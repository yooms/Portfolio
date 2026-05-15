#include "AbilitySystem/GameplayTags/KMGameplayTags.h"
#include "GameplayTagsManager.h"

FGameplayTag KMGameplayTags::Ability_Fire;
FGameplayTag KMGameplayTags::Ability_Aim;
FGameplayTag KMGameplayTags::Ability_Jump;
FGameplayTag KMGameplayTags::Ability_Roll;
FGameplayTag KMGameplayTags::Ability_HomingFire;

FGameplayTag KMGameplayTags::Ability_HatSkill[10];
FGameplayTag KMGameplayTags::Ability_CommandInput;

FGameplayTag KMGameplayTags::Input_Fire;
FGameplayTag KMGameplayTags::Input_HatFire;
FGameplayTag KMGameplayTags::Input_CommandUp;
FGameplayTag KMGameplayTags::Input_CommandDown;
FGameplayTag KMGameplayTags::Input_CommandLeft;
FGameplayTag KMGameplayTags::Input_CommandRight;
FGameplayTag KMGameplayTags::Input_SlotSelect[10];

FGameplayTag KMGameplayTags::State_Aiming;
FGameplayTag KMGameplayTags::State_Rolling;
FGameplayTag KMGameplayTags::State_Invincible;
FGameplayTag KMGameplayTags::State_InAir;
FGameplayTag KMGameplayTags::State_CommandInputting;

FGameplayTag KMGameplayTags::Block_Movement;
FGameplayTag KMGameplayTags::Block_BasicFire;

FGameplayTag KMGameplayTags::Event_Fire;
FGameplayTag KMGameplayTags::Event_CommandSuccess;

void KMGameplayTags::InitTags()
{
    UGameplayTagsManager& Mgr = UGameplayTagsManager::Get();

    // 기존 태그
    Ability_Fire       = Mgr.AddNativeGameplayTag(TEXT("Character.Projectile.BasicSkill"));
    Ability_Aim        = Mgr.AddNativeGameplayTag(TEXT("Character.Sight"));
    Ability_Jump       = Mgr.AddNativeGameplayTag(TEXT("Character.Motion.Jump"));
    Ability_Roll       = Mgr.AddNativeGameplayTag(TEXT("Character.Motion.Roll"));
    Ability_HomingFire = Mgr.AddNativeGameplayTag(TEXT("Character.Projectile.Homing"));

    // 모자 슬롯 스킬 태그 (Hat.Skill.Slot0 ~ Hat.Skill.Slot9)
    for (int32 i = 0; i < 10; i++)
    {
        Ability_HatSkill[i] = Mgr.AddNativeGameplayTag(
            *FString::Printf(TEXT("Hat.Skill.Slot%d"), i));
    }
    Ability_CommandInput = Mgr.AddNativeGameplayTag(TEXT("Hat.CommandInput"));

    // 입력 태그
    Input_Fire         = Mgr.AddNativeGameplayTag(TEXT("Input.Fire"));
    Input_HatFire      = Mgr.AddNativeGameplayTag(TEXT("Input.HatFire"));
    Input_CommandUp    = Mgr.AddNativeGameplayTag(TEXT("Input.Command.Up"));
    Input_CommandDown  = Mgr.AddNativeGameplayTag(TEXT("Input.Command.Down"));
    Input_CommandLeft  = Mgr.AddNativeGameplayTag(TEXT("Input.Command.Left"));
    Input_CommandRight = Mgr.AddNativeGameplayTag(TEXT("Input.Command.Right"));

    // 슬롯 선택 태그 (Input.SlotSelect.0 ~ Input.SlotSelect.9)
    for (int32 i = 0; i < 10; i++)
    {
        Input_SlotSelect[i] = Mgr.AddNativeGameplayTag(
            *FString::Printf(TEXT("Input.SlotSelect.%d"), i));
    }

    // 상태 태그
    State_Aiming          = Mgr.AddNativeGameplayTag(TEXT("Character.State.Aiming"));
    State_Rolling         = Mgr.AddNativeGameplayTag(TEXT("Character.State.Rolling"));
    State_Invincible      = Mgr.AddNativeGameplayTag(TEXT("Character.State.Invincible"));
    State_InAir           = Mgr.AddNativeGameplayTag(TEXT("Character.State.InAir"));
    State_CommandInputting = Mgr.AddNativeGameplayTag(TEXT("Character.State.CommandInputting"));

    // 블록 태그
    Block_Movement  = Mgr.AddNativeGameplayTag(TEXT("Character.Block.Movement"));
    Block_BasicFire = Mgr.AddNativeGameplayTag(TEXT("Character.Block.BasicFire"));

    // 이벤트 태그
    Event_Fire           = Mgr.AddNativeGameplayTag(TEXT("Character.Event.Fire"));
    Event_CommandSuccess = Mgr.AddNativeGameplayTag(TEXT("Character.Event.CommandSuccess"));
}
