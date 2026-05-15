#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KMGameMode.generated.h"

UCLASS(abstract)
class KEYBOARDMAGE_API AKMGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AKMGameMode();

    virtual void PostLogin(APlayerController* NewPlayer) override;

    /** 플레이어에게 랜덤 커맨드 부여. 발사 후 재호출됨. */
    void AssignRandomCommand(class AKMPlayerState* PS);

    /** 커맨드 길이 (화살표 입력 횟수) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stratagem", meta=(ClampMin="1", ClampMax="8"))
    int32 CommandLength = 4;
};
