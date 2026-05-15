#pragma once

#include "CoreMinimal.h"
#include "Projectile/KMProjectile.h"
#include "KMStaticProjectile.generated.h"

UCLASS()
class KEYBOARDMAGE_API AKMStaticProjectile : public AKMProjectile
{
    GENERATED_BODY()

public:
    AKMStaticProjectile();

protected:
    UPROPERTY(EditDefaultsOnly)
    class UStaticMeshComponent* StaticMesh;

};