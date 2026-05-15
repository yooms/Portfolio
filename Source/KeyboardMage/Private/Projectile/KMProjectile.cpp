#include "Projectile/KMProjectile.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"


AKMProjectile::AKMProjectile()
{
    bReplicates = true;
}

void AKMProjectile::SetSourceASC(UAbilitySystemComponent* InASC)
{
    SourceASC = InASC;
}

void AKMProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!IsValid(OtherActor) || OtherActor == GetOwner())
        return;

    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
    if (TargetASC && DamageEffect && SourceASC)
    {
        FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
        Context.AddSourceObject(this);

        FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffect, 1.f, Context);

        if (Spec.IsValid())
        {
            TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
        }
    }

    Destroy();
}