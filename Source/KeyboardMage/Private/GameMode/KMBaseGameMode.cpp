// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMode/KMBaseGameMode.h"

AKMBaseGameMode::AKMBaseGameMode()
{
	// stub
}

void AKMBaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("Player Connected: %s"), *NewPlayer->GetName());
}
