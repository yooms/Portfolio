// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KMBaseGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class KEYBOARDMAGE_API AKMBaseGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKMBaseGameMode();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};



