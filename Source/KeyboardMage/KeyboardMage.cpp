// Copyright Epic Games, Inc. All Rights Reserved.

#include "KeyboardMage.h"
#include "Modules/ModuleManager.h"
#include "AbilitySystem/GameplayTags/KMGameplayTags.h"

// [추가] 기본 FDefaultGameModuleImpl 대신 커스텀 모듈 클래스 사용.
// StartupModule 에서 KMGameplayTags::InitTags() 를 호출하여
// 게임 시작 시 모든 네이티브 GameplayTag 를 등록함.
// 이렇게 하면 에디터/게임 어디서든 KMGameplayTags::Ability_Fire 등을
// 바로 사용할 수 있고, 문자열 오타로 인한 런타임 오류를 방지함.
class FKeyboardMageModule : public FDefaultGameModuleImpl
{
public:
    virtual void StartupModule() override
    {
        FDefaultGameModuleImpl::StartupModule();

        // 네이티브 GameplayTag 일괄 등록
        KMGameplayTags::InitTags();
    }
};

IMPLEMENT_PRIMARY_GAME_MODULE(FKeyboardMageModule, KeyboardMage, "KeyboardMage");

DEFINE_LOG_CATEGORY(LogKeyboardMage)
