#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "KMBaseGameInstance.generated.h"

UCLASS(abstract)
class KEYBOARDMAGE_API UKMBaseGameInstance : public UGameInstance
{
    GENERATED_BODY()


public:
    virtual void Init() override;
    virtual void Shutdown() override;
    virtual void StartGameInstance() override;
};