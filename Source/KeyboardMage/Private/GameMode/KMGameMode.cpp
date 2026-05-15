#include "GameMode/KMGameMode.h"
#include "AbilitySystem/GameplayState/KMPlayerState.h"
#include "Common/KMSpellratagemTypes.h"

AKMGameMode::AKMGameMode()
{
}

void AKMGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UE_LOG(LogTemp, Warning, TEXT("Player Connected: %s"), *NewPlayer->GetName());

    if (AKMPlayerState* PS = NewPlayer->GetPlayerState<AKMPlayerState>())
    {
        AssignRandomCommand(PS);
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("[KM] PostLogin: PlayerState 가 AKMPlayerState 가 아닙니다.\n"
                 "BP_GameMode → Class Defaults → Player State Class 를 "
                 "AKMPlayerState 로 설정하세요."));
    }
}

void AKMGameMode::AssignRandomCommand(AKMPlayerState* PS)
{
    if (!IsValid(PS)) return;

    const TArray<EKMCommandDirection> Pool = {
        EKMCommandDirection::Up,
        EKMCommandDirection::Down,
        EKMCommandDirection::Left,
        EKMCommandDirection::Right
    };

    FStratagemCommand Cmd;
    for (int32 i = 0; i < CommandLength; i++)
    {
        Cmd.Inputs.Add(Pool[FMath::RandRange(0, Pool.Num() - 1)]);
    }

    PS->SetCommand(Cmd);

    FString Seq;
    for (EKMCommandDirection D : Cmd.Inputs)
    {
        Seq += (D == EKMCommandDirection::Up)   ? TEXT("↑") :
               (D == EKMCommandDirection::Down)  ? TEXT("↓") :
               (D == EKMCommandDirection::Left)  ? TEXT("←") : TEXT("→");
        Seq += TEXT(" ");
    }
    UE_LOG(LogTemp, Warning, TEXT("[KM] Command to [%s]: %s"),
        *PS->GetPlayerName(), *Seq.TrimEnd());
}
