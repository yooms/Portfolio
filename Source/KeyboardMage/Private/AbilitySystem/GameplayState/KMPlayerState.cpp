#include "AbilitySystem/GameplayState/KMPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/KMGameMode.h"

AKMPlayerState::AKMPlayerState()
{
}

void AKMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AKMPlayerState, CurrentCommand);
    DOREPLIFETIME(AKMPlayerState, bCommandReady);
}

void AKMPlayerState::SetCommand(const FStratagemCommand& NewCommand)
{
    CurrentCommand = NewCommand;
    bCommandReady  = false;
    // 서버 로컬에도 즉시 이벤트 전파 (OnRep 는 클라이언트에서만 자동 호출)
    OnRep_CurrentCommand();
    OnRep_CommandReady();
}

void AKMPlayerState::ConsumeAndRequestNew()
{
    bCommandReady = false;
    OnRep_CommandReady();

    if (UWorld* World = GetWorld())
    {
        if (AKMGameMode* GM = Cast<AKMGameMode>(World->GetAuthGameMode()))
        {
            GM->AssignRandomCommand(this);
        }
    }
}

void AKMPlayerState::Server_SubmitCommand_Implementation(
    const TArray<EKMCommandDirection>& InputSequence)
{
    // 서버에서 CurrentCommand 와 입력 비교 후 bCommandReady 설정
    const bool bMatch = CurrentCommand.Matches(InputSequence);

    UE_LOG(LogTemp, Warning, TEXT("[KM] Server_SubmitCommand - Match:%d (%d/%d inputs)"), (int)bMatch, InputSequence.Num(), CurrentCommand.Inputs.Num());

    bCommandReady = bMatch;
    // OnRep 는 클라이언트에 자동 복제 후 호출됨
    // 서버 로컬에도 즉시 호출
    OnRep_CommandReady();
}

void AKMPlayerState::OnRep_CurrentCommand()
{
    OnCommandChanged.Broadcast(CurrentCommand);

    FString Seq;
    for (EKMCommandDirection D : CurrentCommand.Inputs)
    {
        Seq += (D == EKMCommandDirection::Up)   ? TEXT("↑") :
               (D == EKMCommandDirection::Down)  ? TEXT("↓") :
               (D == EKMCommandDirection::Left)  ? TEXT("←") : TEXT("→");
        Seq += TEXT(" ");
    }
    UE_LOG(LogTemp, Warning, TEXT("[KM] New Command: [ %s]"), *Seq);
}

void AKMPlayerState::OnRep_CommandReady()
{
    OnCommandResult.Broadcast(bCommandReady);
    UE_LOG(LogTemp, Warning, TEXT("[KM] CommandReady: %s"), bCommandReady ? TEXT("✅ 발사 가능") : TEXT("❌ 커맨드 필요"));
}
