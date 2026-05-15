#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputActionValue.h"
#include "Common/KMSpellratagemTypes.h"
#include "Common/KMHatTypes.h"
#include "Abilities/GameplayAbility.h"
#include "InputMappingContext.h"
#include "KMInputHandlerComponent.generated.h"


class UInputAction;
class UEnhancedInputComponent;
class AKMCharacter;


/**
 * KMInputHandlerComponent (리팩토링)
 *
 * 3개의 IMC(InputMappingContext)로 입력을 분리 관리:
 *
 *   [1] IMC_Movement  — 항상 활성
 *       Move, Look, MouseLook, Jump, Roll, Aim
 *
 *   [2] IMC_Command   — 항상 활성 (화살표 키)
 *       CommandUp, CommandDown, CommandLeft, CommandRight
 *       → KMHatComponent 에 전달하여 커맨드 매칭
 *
 *   [3] IMC_SlotSelect — 항상 활성 (숫자키 0~9)
 *       SlotSelect0 ~ SlotSelect9
 *       → KMHatComponent::SelectSlot() 호출
 *
 * LMB(Fire) 동작:
 *   - 슬롯 미선택 / 커맨드 미완성: KMGA_Fire 활성화 (기본 약한 원거리 공격)
 *   - 슬롯 선택 + 커맨드 완성:    KMHatComponent::ActivateSelectedSlotSkill()
 *
 * GAS 연동:
 *   모든 GA 활성화는 TryActivateAbilityByTag / TryActivateAbility 를 사용.
 *   InputID 기반이 아닌 GameplayTag 기반으로 동작.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KEYBOARDMAGE_API UKMInputHandlerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    void Initialize(UEnhancedInputComponent* InputComp, AKMCharacter* InOwner);

    // ── [1] 이동 IMC ──────────────────────────────────────────────────────

    /**
     * 이동 관련 InputMappingContext.
     * Move / Look / MouseLook / Jump / Roll / Aim 액션을 포함.
     * Priority: 0 (가장 낮음, 항상 활성)
     */
    UPROPERTY(EditAnywhere, Category="Input|IMC", meta=(DisplayName="IMC_Movement"))
    TObjectPtr<UInputMappingContext> IMC_Movement;

    /**
     * 화살표 커맨드 InputMappingContext.
     * CommandUp / Down / Left / Right 액션을 포함.
     * Priority: 1
     */
    UPROPERTY(EditAnywhere, Category="Input|IMC", meta=(DisplayName="IMC_Command"))
    TObjectPtr<UInputMappingContext> IMC_Command;

    /**
     * 숫자키 슬롯 선택 InputMappingContext.
     * Slot0 ~ Slot9 액션 포함.
     * Priority: 2
     */
    UPROPERTY(EditAnywhere, Category="Input|IMC", meta=(DisplayName="IMC_SlotSelect"))
    TObjectPtr<UInputMappingContext> IMC_SlotSelect;

    // ── [1] 이동 IMC 액션 ─────────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category="Input|Actions|Movement") UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Movement") UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Movement") UInputAction* LookAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Movement") UInputAction* MouseLookAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Movement") UInputAction* RollAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Movement") UInputAction* AimAction;
    /** LMB — 기본 발사 or 모자 스킬 발사 (컨텍스트에 따라 분기) */
    UPROPERTY(EditAnywhere, Category="Input|Actions|Movement") UInputAction* FireAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Movement") UInputAction* HomingFireAction;

    // ── [2] 커맨드 IMC 액션 ───────────────────────────────────────────────
    UPROPERTY(EditAnywhere, Category="Input|Actions|Command") UInputAction* CommandUpAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Command") UInputAction* CommandDownAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Command") UInputAction* CommandLeftAction;
    UPROPERTY(EditAnywhere, Category="Input|Actions|Command") UInputAction* CommandRightAction;

    // ── [3] 슬롯 선택 IMC 액션 ───────────────────────────────────────────
    /** 숫자키 1~9,0 → 슬롯 선택 액션 배열 (인덱스 0=슬롯0(키1), ..., 8=슬롯8(키9), 9=슬롯9(키0)) */
    UPROPERTY(EditAnywhere, Category="Input|Actions|SlotSelect")
    TArray<UInputAction*> SlotSelectActions;

private:
    AKMCharacter* OwnerCharacter = nullptr;

    // 커맨드 입력 리셋 시간 (HatComponent 에서 관리하지만 여기서도 유지)
    UPROPERTY(EditDefaultsOnly, Category="Input")
    float InputResetTime = 1.5f;

    // ── 입력 핸들러 함수 ──────────────────────────────────────────────────

    void JumpStart();
    void JumpEnd();
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

    /** LMB Pressed */
    void FirePressed();
    /** LMB Released — 컨텍스트에 따라 기본 발사 or 모자 스킬 발사 */
    void FireReleased();

    void AimPressed();
    void AimReleased();
    void Roll();
    void HomingFire();

    // 화살표 커맨드 입력 → HatComponent 로 전달
    void CommandUp   (const FInputActionValue& Value);
    void CommandDown (const FInputActionValue& Value);
    void CommandLeft (const FInputActionValue& Value);
    void CommandRight(const FInputActionValue& Value);

    /** 숫자키 슬롯 선택 (액션 인덱스로부터 슬롯 인덱스 계산) */
    void OnSlotSelect(int32 SlotIndex);

    // ── GAS 헬퍼 ─────────────────────────────────────────────────────────
    void TryActivateAbilityByClass(TSubclassOf<UGameplayAbility> AbilityClass);
    void SendAbilityInputReleased(TSubclassOf<UGameplayAbility> AbilityClass);

    // IMC 등록 헬퍼
    void RegisterIMCs();

	UFUNCTION()
	void InputSlot0(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot1(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot2(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot3(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot4(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot5(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot6(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot7(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot8(const FInputActionValue& Value);

	UFUNCTION()
	void InputSlot9(const FInputActionValue& Value);

};
