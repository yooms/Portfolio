#pragma once

#include "CoreMinimal.h"
#include "Projectile/KMProjectile.h"
#include "KMSkeletalProjectile.generated.h"

UCLASS()
class KEYBOARDMAGE_API AKMSkeletalProjectile : public AKMProjectile
{
    GENERATED_BODY()

public:
    AKMSkeletalProjectile();

protected:
    UPROPERTY(EditDefaultsOnly)
    class USkeletalMeshComponent* SkeletalMesh;

};