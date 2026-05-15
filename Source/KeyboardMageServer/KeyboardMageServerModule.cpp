#include "KeyboardMageServerModule.h"

void FKeyboardMageServerModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("Server Module Started"));
}

void FKeyboardMageServerModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FKeyboardMageServerModule, KeyboardMageServer)