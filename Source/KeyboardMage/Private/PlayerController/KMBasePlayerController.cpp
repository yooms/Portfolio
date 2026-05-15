// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlayerController/KMBasePlayerController.h"



AKMBasePlayerController::AKMBasePlayerController()
{

}

void AKMBasePlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void AKMBasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

}

bool AKMBasePlayerController::ShouldUseTouchControls() const
{
	return false;
}
