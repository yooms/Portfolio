#include "GameInstance/KMBaseGameInstance.h"
#include "Engine/Engine.h"


void UKMBaseGameInstance::Init()
{
    Super::Init();

    UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
}

void UKMBaseGameInstance::Shutdown()
{
    UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown"));

    Super::Shutdown();
}

void UKMBaseGameInstance::StartGameInstance()
{
    Super::StartGameInstance();


}
