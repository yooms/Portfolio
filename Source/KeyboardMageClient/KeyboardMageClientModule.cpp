#include "KeyboardMageClientModule.h"

void FKeyboardMageClientModule::StartupModule()
{
    UE_LOG(LogTemp, Warning, TEXT("Client Module Started"));
}

void FKeyboardMageClientModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FKeyboardMageClientModule, KeyboardMageClient)