#pragma once

#include "Modules/ModuleManager.h"

class FKeyboardMageClientModule : public IModuleInterface
{
public:
    static inline FKeyboardMageClientModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FKeyboardMageClientModule>("KeyboardMageClient");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("KeyboardMageClient");
    }

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};