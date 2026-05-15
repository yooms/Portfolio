#include "Projectile/KMBaseProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AKMBaseProjectile::AKMBaseProjectile()
{
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = false;

    Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->InitSphereRadius(15.f);
    Collision->SetCollisionProfileName(TEXT("BlockAll"));
    Collision->SetNotifyRigidBodyCollision(true);
    Collision->SetGenerateOverlapEvents(true);
    Collision->OnComponentHit.AddDynamic(this, &AKMBaseProjectile::HandleHit);

    Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
    Movement->InitialSpeed = 2000.f;
    Movement->MaxSpeed = 2000.f;
    Movement->bRotationFollowsVelocity = true;
    Movement->bShouldBounce = false;

    InitialLifeSpan = 5.f;
}

void AKMBaseProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void AKMBaseProjectile::HandleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
 