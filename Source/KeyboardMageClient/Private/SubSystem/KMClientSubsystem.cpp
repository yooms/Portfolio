#include "SubSystem/KMClientSubsystem.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"


void UKMClientSubsystem::InitUI()
{
    UE_LOG(LogTemp, Warning, TEXT("UI Initialized"));
}

void UKMClientSubsystem::OnConnectToGameServer()
{
    UE_LOG(LogTemp, Warning, TEXT("Connecting to Game Server..."));

    ENetMode NetMode = ENetMode::NM_MAX;
    UWorld* CurrentWorld = GetWorld();
    if (IsValid(CurrentWorld))
    {
        NetMode = CurrentWorld->GetNetMode();
    }

    if (IsValid(GetWorld()))
    {
        FString ServerAddress = TEXT("127.0.0.1");

        UE_LOG(LogTemp, Warning, TEXT("Connecting to %s"), *ServerAddress);

        UGameplayStatics::OpenLevel(GetWorld(), FName(*ServerAddress), true);
    }
}

void UKMClientSubsystem::ConnectToGameServer()
{
    OnConnectToGameServer();
}