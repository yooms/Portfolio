#pragma once

#include "CoreMinimal.h"
#include "Projectile/KMBaseProjectile.h"
#include "KMProjectile.generated.h"

UCLASS()
class KEYBOARDMAGE_API AKMProjectile : public AKMBaseProjectile
{
    GENERATED_BODY()

public:
    AKMProjectile();

    void SetSourceASC(class UAbilitySystemComponent* InASC);

protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<class UGameplayEffect> DamageEffect;

    UPROPERTY()
    class UAbilitySystemComponent* SourceASC;

protected:
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};