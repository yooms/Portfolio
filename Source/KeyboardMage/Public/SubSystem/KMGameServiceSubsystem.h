#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KMGameServiceSubsystem.generated.h"

UCLASS()
class KEYBOARDMAGE_API UKMGameServiceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

protected:
    virtual void OnInit() {}
    virtual void OnShutdown() {}

public:
    virtual void ConnectToGameServer() {}
};