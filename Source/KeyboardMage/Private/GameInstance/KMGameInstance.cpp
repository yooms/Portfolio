#include "GameInstance/KMGameInstance.h"
#include "Engine/Engine.h"
#include "SubSystem/KMGameServiceSubsystem.h"
#include "Modules/ModuleManager.h"
#include "SubSystem/KMClientInterface.h"

//#if !UE_SERVER
//#include "SubSystem/KMClientSubsystem.h"
//#endif


void UKMGameInstance::Init()
{
    Super::Init();

    UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
}

void UKMGameInstance::Shutdown()
{
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown"));

    Super::Shutdown();
}

void UKMGameInstance::StartGameInstance()
{
    Super::StartGameInstance();

}

void UKMGameInstance::OnStart()
{
    Super::OnStart();

    if (IsRunningDedicatedServer())
        return;

    if (!FModuleManager::Get().IsModuleLoaded("KeyboardMageClient"))
        return;

#if !UE_SERVER
//    UGameInstanceSubsystem* Subsystem = GetSubsystemBase(UGameInstanceSubsystem::StaticClass());
//   IKMClientInterface* Interface = Cast<IKMClientInterface>(Subsystem);

//    if (Interface)
//    {
//        Interface->ConnectToGameServer();
//    }

    if (IKMClientInterface* Client = IKMClientInterface::Get(this))
    {
        Client->ConnectToGameServer();
    }
#endif

    //UKMGameServiceSubsystem* Subsystem = GetSubsystem<UKMGameServiceSubsystem>();
    //if (Subsystem)
    //{
    //    Subsystem->ConnectToGameServer();
    //}
}