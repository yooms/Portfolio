#pragma once

#include "Modules/ModuleManager.h"

class FKeyboardMageServerModule : public IModuleInterface
{
public:
    static inline FKeyboardMageServerModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FKeyboardMageServerModule>("KeyboardMageServer");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("KeyboardMageServer");
    }

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};