#include "Character/KMCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "KeyboardMage/KeyboardMage.h"
#include "Net/UnrealNetwork.h"
#include "Character/Components/KMInputHandlerComponent.h"
#include "Character/Components/KMSkillIndicatorComponent.h"
#include "Character/Components/KMHatComponent.h"
#include "Projectile/KMProjectile.h"
#include "AbilitySystemComponent.h"


AKMCharacter::AKMCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw   = false;
    bUseControllerRotationRoll  = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate               = FRotator(0.f, 500.f, 0.f);
    GetCharacterMovement()->JumpZVelocity              = 500.f;
    GetCharacterMovement()->AirControl                 = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed               = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed         = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength         = DefaultBoomLength;
    CameraBoom->SocketOffset            = DefaultSocketOffset;
    CameraBoom->TargetOffset            = FVector(0.f, 0.f, 45.f);
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    InputHandler   = CreateDefaultSubobject<UKMInputHandlerComponent>(TEXT("InputHandler"));
    SkillIndicator = CreateDefaultSubobject<UKMSkillIndicatorComponent>(TEXT("SkillIndicator"));

    // 모자 컴포넌트 생성
    HatComponent = CreateDefaultSubobject<UKMHatComponent>(TEXT("HatComponent"));

    bReplicates = true;
    PrimaryActorTick.bCanEverTick = true;
}

void AKMCharacter::BeginPlay()
{
    Super::BeginPlay();
    CurrentFOV = DefaultFOV;
    if (FollowCamera)
        FollowCamera->SetFieldOfView(CurrentFOV);

    // HatComponent 초기화 (캐릭터 레퍼런스 전달)
    if (HatComponent)
        HatComponent->Initialize(this);
}

void AKMCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (IsLocallyControlled())
    {
        UpdateAimPoint();
        UpdateAimOffset();

        if (!HasAuthority())
            UpdateReplicatedAimInfo();
        else
        {
            ReplicatedAimPoint = AimPoint;
            if (FollowCamera)
            {
                ReplicatedCameraLocation = FollowCamera->GetComponentLocation();
                ReplicatedCameraRotation = FollowCamera->GetComponentRotation();
            }
        }
    }

    UpdateTurnInPlace(DeltaSeconds);
    UpdateRotation(DeltaSeconds);
    UpdateCameraForAim(DeltaSeconds);
}

void AKMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (InputHandler) InputHandler->Initialize(EIC, this);
        else UE_LOG(LogTemp, Error, TEXT("[KM] InputHandler is NULL"));
    }
}

void AKMCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AKMCharacter, AimRotation);
    DOREPLIFETIME(AKMCharacter, AimPitch);
    DOREPLIFETIME(AKMCharacter, AimYaw);
    DOREPLIFETIME(AKMCharacter, bIsAim);
    DOREPLIFETIME(AKMCharacter, ReplicatedAimPoint);
    DOREPLIFETIME(AKMCharacter, ReplicatedCameraLocation);
    DOREPLIFETIME(AKMCharacter, ReplicatedCameraRotation);
}

void AKMCharacter::Move(const FInputActionValue& Value)
{
    FVector2D V = Value.Get<FVector2D>(); DoMove(V.X, V.Y);
}
void AKMCharacter::Look(const FInputActionValue& Value)
{
    FVector2D V = Value.Get<FVector2D>(); DoLook(V.X, V.Y);
}

void AKMCharacter::DoMove(float Right, float Forward)
{
    if (!GetController()) return;
    const FRotator YawRot(0, GetController()->GetControlRotation().Yaw, 0);
    AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::X), Forward);
    AddMovementInput(FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y), Right);
}

void AKMCharacter::DoLook(float Yaw, float Pitch)
{
    if (!GetController()) return;
    AddControllerYawInput(Yaw);
    AddControllerPitchInput(Pitch);
}

void AKMCharacter::DoJumpStart() { Jump(); }
void AKMCharacter::DoJumpEnd()   { StopJumping(); }

void AKMCharacter::DoFire()
{
    if (IsLocallyControlled()) { UpdateAimPoint(); Server_Fire(AimPoint); }
}
void AKMCharacter::DoAim()
{
    bIsAim = !bIsAim;
}

void AKMCharacter::Server_SetAim_Implementation(bool bAiming)
{
    bIsAim = bAiming;
}

void AKMCharacter::UpdateAimPoint()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;
    int32 SX, SY;
    PC->GetViewportSize(SX, SY);
    FVector Loc, Dir;
    if (!PC->DeprojectScreenPositionToWorld(SX * 0.5f, SY * 0.5f, Loc, Dir)) return;
    FHitResult Hit;
    FCollisionQueryParams P; P.AddIgnoredActor(this);
    GetWorld()->LineTraceSingleByChannel(Hit, Loc, Loc + Dir * 15000.f, ECC_Visibility, P);
    AimPoint = Hit.bBlockingHit ? Hit.ImpactPoint : (Loc + Dir * 15000.f);
}

void AKMCharacter::UpdateReplicatedAimInfo()
{
    AimInfoSendAccumulator += GetWorld()->GetDeltaSeconds();
    if (AimInfoSendAccumulator < AimInfoSendInterval) return;
    AimInfoSendAccumulator = 0.f;
    if (AimPoint.IsZero()) return;
    FVector CamLoc; FRotator CamRot;
    if (FollowCamera) { CamLoc = FollowCamera->GetComponentLocation(); CamRot = FollowCamera->GetComponentRotation(); }
    Server_UpdateAimInfo(AimPoint, CamLoc, CamRot);
}

void AKMCharacter::Server_UpdateAimInfo_Implementation(const FVector& InAimPoint,
    const FVector& InCameraLocation, const FRotator& InCameraRotation)
{
    if (FVector::Dist(GetActorLocation(), InAimPoint) > 20000.f) return;
    ReplicatedAimPoint       = InAimPoint;
    ReplicatedCameraLocation = InCameraLocation;
    ReplicatedCameraRotation = InCameraRotation;
}

void AKMCharacter::UpdateRotation(float DeltaTime)
{
    AController* C = GetController();
    if (!C) return;
    const bool bMoving = !GetVelocity().IsNearlyZero(1.f);
    if (bMoving)
    {
        const FRotator Target(0.f, C->GetControlRotation().Yaw, 0.f);
        SetActorRotation(FMath::RInterpTo(GetActorRotation(), Target, DeltaTime, RotationSpeed));
    }
}

void AKMCharacter::UpdateTurnInPlace(float DeltaTime)
{
    AController* C = GetController();
    if (!C) return;
    const bool bMoving = !GetVelocity().IsNearlyZero(1.f);
    const bool bInAir  = GetCharacterMovement()->IsFalling();
    if (bMoving || bInAir) { bIsTurningInPlace = false; TurnInPlaceYaw = 0.f; return; }

    const float ControlYaw   = C->GetControlRotation().Yaw;
    const float CharacterYaw = GetActorRotation().Yaw;
    const float DeltaYaw     = FMath::FindDeltaAngleDegrees(CharacterYaw, ControlYaw);
    TurnInPlaceYaw = DeltaYaw;

    if (FMath::Abs(DeltaYaw) > TurnInPlaceThreshold)
    {
        bIsTurningInPlace = true;
        const float MaxTurn   = TurnInPlaceSpeed * DeltaTime;
        const float TurnDelta = FMath::Clamp(DeltaYaw, -MaxTurn, MaxTurn);
        SetActorRotation(FRotator(0.f, CharacterYaw + TurnDelta, 0.f));
    }
    else
    {
        bIsTurningInPlace = false;
    }
}

void AKMCharacter::UpdateAimOffset()
{
    if (AimPoint.IsZero()) return;
    const FRotator AR = (AimPoint - GetActorLocation()).GetSafeNormal().Rotation();
    AimPitch    = FMath::ClampAngle(AR.Pitch, -90.f, 90.f);
    AimYaw      = FMath::ClampAngle(FMath::FindDeltaAngleDegrees(GetActorRotation().Yaw, AR.Yaw), -90.f, 90.f);
    AimRotation = AR;
}

void AKMCharacter::UpdateCameraForAim(float DeltaTime)
{
    if (!IsLocallyControlled() || !CameraBoom || !FollowCamera) return;
    CameraBoom->TargetArmLength = FMath::FInterpTo(
        CameraBoom->TargetArmLength, bIsAim ? AimBoomLength : DefaultBoomLength, DeltaTime, AimCameraInterpSpeed);
    CameraBoom->SocketOffset = FMath::VInterpTo(
        CameraBoom->SocketOffset, bIsAim ? AimSocketOffset : DefaultSocketOffset, DeltaTime, AimCameraInterpSpeed);
    CurrentFOV = FMath::FInterpTo(CurrentFOV, bIsAim ? AimFOV : DefaultFOV, DeltaTime, ZoomInterpSpeed);
    FollowCamera->SetFieldOfView(CurrentFOV);
}

FVector AKMCharacter::GetMuzzleLocation() const
{
    if (GetMesh() && GetMesh()->DoesSocketExist(TEXT("Muzzle")))
        return GetMesh()->GetSocketLocation(TEXT("Muzzle"));
    return GetActorLocation() + GetActorForwardVector() * 100.f;
}

void AKMCharacter::Server_Fire_Implementation(const FVector& InAimPoint)
{
    Multicast_Fire(InAimPoint);
}

bool AKMCharacter::Server_Fire_Validate(const FVector& InAimPoint)
{
    return FVector::Dist(GetActorLocation(), InAimPoint) <= 20000.f;
}

void AKMCharacter::Multicast_Fire_Implementation(const FVector& InAimPoint)
{
    if (!ProjectileClass) return;
    const FVector ML  = GetMuzzleLocation();
    const FVector Dir = (InAimPoint - ML).GetSafeNormal();
    FActorSpawnParameters P; P.Owner = this; P.Instigator = GetInstigator();
    if (AKMProjectile* Proj = GetWorld()->SpawnActor<AKMProjectile>(ProjectileClass, ML, Dir.Rotation(), P))
        Proj->SetSourceASC(GetAbilitySystemComponent());
}
