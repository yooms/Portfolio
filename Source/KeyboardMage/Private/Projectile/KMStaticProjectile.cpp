#include "Projectile/KMStaticProjectile.h"
#include "Components/StaticMeshComponent.h"


AKMStaticProjectile::AKMStaticProjectile()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshProjectile");
	StaticMesh->SetupAttachment(Collision);
}

