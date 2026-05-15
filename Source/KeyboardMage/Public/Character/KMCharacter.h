#pragma once

#include "CoreMinimal.h"
#include "KMBaseCharacter.h"
#include "KMCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
class UKMSkillIndicatorComponent;
class UKMHatComponent;


UCLASS()
class KEYBOARDMAGE_API AKMCharacter : public AKMBaseCharacter
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere)
    class UKMInputHandlerComponent* InputHandler;

    UPROPERTY(VisibleAnywhere)
    UKMSkillIndicatorComponent* SkillIndicator;

    /**
     * 모자 관리 컴포넌트.
     * 최대 10개 슬롯, 각 슬롯에 모자 스킬 GA + 커맨드 시퀀스 저장.
     * BP에서 초기 장착 모자를 DefaultHats 배열로 설정하거나,
     * 런타임에 HatComponent->EquipHat() 호출로 장착.
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
    UKMHatComponent* HatComponent;

public:
    AKMCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);

public:
    UFUNCTION(BlueprintCallable, Category="Input") virtual void DoMove(float Right, float Forward);
    UFUNCTION(BlueprintCallable, Category="Input") virtual void DoLook(float Yaw, float Pitch);
    UFUNCTION(BlueprintCallable, Category="Input") virtual void DoJumpStart();
    UFUNCTION(BlueprintCallable, Category="Input") virtual void DoJumpEnd();

    FORCEINLINE USpringArmComponent* GetCameraBoom()   const { return CameraBoom; }
    FORCEINLINE UCameraComponent*    GetFollowCamera()  const { return FollowCamera; }
    FORCEINLINE UKMHatComponent*     GetHatComponent()  const { return HatComponent; }
    FORCEINLINE float   GetAimPitch()  const { return AimPitch; }
    FORCEINLINE float   GetAimYaw()    const { return AimYaw; }
    FORCEINLINE bool    IsAim()        const { return bIsAim; }
    FORCEINLINE void    SetAim(bool A)       { bIsAim = A; }
    FORCEINLINE FVector GetCurrentAimPoint()        const { return AimPoint; }
    FORCEINLINE FVector GetReplicatedAimPoint()     const { return ReplicatedAimPoint; }
    FORCEINLINE FVector    GetReplicatedCameraLocation() const { return ReplicatedCameraLocation; }
    FORCEINLINE FRotator   GetReplicatedCameraRotation() const { return ReplicatedCameraRotation; }

    UFUNCTION(BlueprintPure, Category="Animation")
    float GetTurnInPlaceYaw() const { return TurnInPlaceYaw; }

    UFUNCTION(BlueprintPure, Category="Animation")
    bool IsTurningInPlace() const { return bIsTurningInPlace; }

public:
    void DoFire();
    void DoAim();

    UFUNCTION(Server, Reliable) void Server_SetAim(bool bAiming);

protected:
    void UpdateAimPoint();
    void UpdateRotation(float DeltaTime);
    void UpdateTurnInPlace(float DeltaTime);
    void UpdateAimOffset();
    void UpdateCameraForAim(float DeltaTime);
    void UpdateReplicatedAimInfo();
    FVector GetMuzzleLocation() const;

public:
    UFUNCTION(Server, Reliable, WithValidation) void Server_Fire(const FVector& InAimPoint);
    UFUNCTION(NetMulticast, Reliable)           void Multicast_Fire(const FVector& InAimPoint);
    UFUNCTION(Server, Unreliable)
    void Server_UpdateAimInfo(const FVector& InAimPoint,
                              const FVector& InCameraLocation,
                              const FRotator& InCameraRotation);

protected:
    UPROPERTY(EditDefaultsOnly) TSubclassOf<class AKMProjectile> ProjectileClass;

    UPROPERTY(Replicated) FRotator AimRotation;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated) float AimPitch = 0.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated) float AimYaw   = 0.f;

    FVector AimPoint = FVector::ZeroVector;
    UPROPERTY(Replicated) FVector   ReplicatedAimPoint       = FVector::ZeroVector;
    UPROPERTY(Replicated) FVector   ReplicatedCameraLocation = FVector::ZeroVector;
    UPROPERTY(Replicated) FRotator  ReplicatedCameraRotation = FRotator::ZeroRotator;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated) bool bIsAim = false;
    UPROPERTY(EditAnywhere, Category="Movement") float RotationSpeed = 8.0f;
    UPROPERTY(EditAnywhere, Category="Movement|TurnInPlace", meta=(ClampMin="0.0", ClampMax="180.0"))
    float TurnInPlaceThreshold = 60.f;

    UPROPERTY(EditAnywhere, Category="Movement|TurnInPlace", meta=(ClampMin="1.0"))
    float TurnInPlaceSpeed = 180.f;

    UPROPERTY(EditAnywhere, Category="Camera") float AimCameraInterpSpeed = 12.0f;
    UPROPERTY(EditAnywhere, Category="Camera") float DefaultBoomLength     = 300.0f;
    UPROPERTY(EditAnywhere, Category="Camera") float AimBoomLength         = 190.0f;
    UPROPERTY(EditAnywhere, Category="Camera") FVector DefaultSocketOffset = FVector(0.f, 30.f, 65.f);
    UPROPERTY(EditAnywhere, Category="Camera") FVector AimSocketOffset     = FVector(15.f, 45.f, 60.f);
    UPROPERTY(EditAnywhere, Category="Camera|Zoom", meta=(ClampMin="30.0", ClampMax="120.0")) float DefaultFOV = 90.f;
    UPROPERTY(EditAnywhere, Category="Camera|Zoom", meta=(ClampMin="20.0", ClampMax="90.0"))  float AimFOV    = 80.f;
    UPROPERTY(EditAnywhere, Category="Camera|Zoom", meta=(ClampMin="1.0"))                    float ZoomInterpSpeed = 15.f;

    UPROPERTY(EditAnywhere, Category="Network", meta=(ClampMin="0.01")) float AimInfoSendInterval = 0.05f;

private:
    float AimInfoSendAccumulator = 0.f;
    float CurrentFOV = 90.f;

    float TurnInPlaceYaw    = 0.f;
    bool  bIsTurningInPlace = false;
};
