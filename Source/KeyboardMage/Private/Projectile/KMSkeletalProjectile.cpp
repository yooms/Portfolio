#include "Projectile/KMSkeletalProjectile.h"
#include "Components/SkeletalMeshComponent.h"


AKMSkeletalProjectile::AKMSkeletalProjectile()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshProjectile");
	SkeletalMesh->SetupAttachment(Collision);
}

