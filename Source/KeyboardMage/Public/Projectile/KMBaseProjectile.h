#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KMBaseProjectile.generated.h"

UCLASS(Abstract)
class KEYBOARDMAGE_API AKMBaseProjectile : public AActor
{
    GENERATED_BODY()

public:
    AKMBaseProjectile();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere)
    class USphereComponent* Collision;

    UPROPERTY(VisibleAnywhere)
    class UProjectileMovementComponent* Movement;

    UFUNCTION()
    void HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {}
};