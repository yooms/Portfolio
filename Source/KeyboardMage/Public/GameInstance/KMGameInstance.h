#pragma once

#include "CoreMinimal.h"
#include "KMBaseGameInstance.h"
#include "KMGameInstance.generated.h"

UCLASS()
class KEYBOARDMAGE_API UKMGameInstance : public UKMBaseGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void StartGameInstance() override;
    virtual void OnStart() override;
};