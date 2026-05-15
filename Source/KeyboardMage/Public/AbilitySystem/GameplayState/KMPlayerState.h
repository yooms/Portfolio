#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayState/KMCharacterState.h"
#include "Common/KMSpellratagemTypes.h"
#include "KMPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCommandChanged, const FStratagemCommand&, NewCommand);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCommandResult,  bool, bSuccess);

UCLASS()
class KEYBOARDMAGE_API AKMPlayerState : public AKMCharacterState
{
    GENERATED_BODY()

public:
    AKMPlayerState();

    // ── 복제 변수 ─────────────────────────────────────────────────────────

    /** 서버가 부여한 현재 커맨드 시퀀스 — 클라이언트 UI 에 표시 */
    UPROPERTY(ReplicatedUsing=OnRep_CurrentCommand, BlueprintReadOnly, Category="Stratagem")
    FStratagemCommand CurrentCommand;

    /** 커맨드 입력 성공 여부 — true 일 때만 LMB 발사 허용 */
    UPROPERTY(ReplicatedUsing=OnRep_CommandReady, BlueprintReadOnly, Category="Stratagem")
    bool bCommandReady = false;

    // ── BP/UI 이벤트 ──────────────────────────────────────────────────────

    UPROPERTY(BlueprintAssignable, Category="Stratagem")
    FOnCommandChanged OnCommandChanged;

    UPROPERTY(BlueprintAssignable, Category="Stratagem")
    FOnCommandResult  OnCommandResult;

    // ── 서버 API (GameMode / GA 에서 호출) ────────────────────────────────

    /** [서버] GameMode 가 새 커맨드를 부여할 때 호출 */
    void SetCommand(const FStratagemCommand& NewCommand);

    /** [서버] 발사 GA 가 발사 성공 후 호출 — bCommandReady 리셋 + 새 커맨드 요청 */
    void ConsumeAndRequestNew();

    // ── 클라이언트 → 서버 RPC ─────────────────────────────────────────────

    /** 클라이언트가 커맨드 입력 완료 시 서버로 전송하여 검증 */
    UFUNCTION(Server, Reliable)
    void Server_SubmitCommand(const TArray<EKMCommandDirection>& InputSequence);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UFUNCTION() void OnRep_CurrentCommand();
    UFUNCTION() void OnRep_CommandReady();
};
