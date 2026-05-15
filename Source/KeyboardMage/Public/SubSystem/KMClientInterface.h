#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/GameInstance.h"
#include "UObject/UObjectGlobals.h"
#include "KMClientInterface.generated.h"

UINTERFACE(BlueprintType)
class KEYBOARDMAGE_API UKMClientInterface : public UInterface
{
    GENERATED_BODY()
};

class KEYBOARDMAGE_API IKMClientInterface
{
    GENERATED_BODY()

public:
    virtual void ConnectToGameServer() = 0;

    // inline으로 구현
    static IKMClientInterface* Get(UObject* WorldContext)
    {
#if !UE_SERVER
        if (!WorldContext)
            return nullptr;

        UWorld* World = WorldContext->GetWorld();
        if (!World)
            return nullptr;

        UGameInstance* GI = World->GetGameInstance();
        if (!GI)
            return nullptr;

        // 문자열 기반 찾기
        UClass* Class = FindObject<UClass>(
            nullptr,
            TEXT("/Script/KeyboardMageClient.KMClientSubsystem")
        );
        if (!Class)
            return nullptr;

        UGameInstanceSubsystem* Sub = GI->GetSubsystemBase(Class);
        if (!Sub)
            return nullptr;

        return Cast<IKMClientInterface>(Sub);
#else
        return nullptr;
#endif
    }
};