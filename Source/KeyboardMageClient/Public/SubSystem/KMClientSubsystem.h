#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KeyboardMage/Public/SubSystem/KMClientInterface.h"
#include "KMClientSubsystem.generated.h"

UCLASS()
class KEYBOARDMAGECLIENT_API UKMClientSubsystem : public UGameInstanceSubsystem, public IKMClientInterface
{
    GENERATED_BODY()

private:
    void InitUI();
    void OnConnectToGameServer();

public:
    virtual void ConnectToGameServer() override;
};