#pragma once
#include "CoreMinimal.h"
#include "Projectile/KMProjectile.h"
#include "KMHomingProjectile.generated.h"



UCLASS()
class KEYBOARDMAGE_API AKMHomingProjectile : public AKMProjectile
{
	GENERATED_BODY()
public:
	AKMHomingProjectile();

	UFUNCTION(BlueprintCallable, Category="Homing")
	void SetHomingTarget(USceneComponent* TargetComponent);

	UPROPERTY(EditDefaultsOnly, Category="Homing", meta=(ClampMin="100.0"))
	float HomingAccelerationMagnitude = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="Homing", meta=(ClampMin="0.0"))
	float HomingDuration = 5.f;

protected:
	virtual void BeginPlay() override;

private:
	void DisableHoming();
	FTimerHandle HomingTimerHandle;
};
