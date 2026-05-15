#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KMHomingComponent.generated.h"

class UProjectileMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class KEYBOARDMAGE_API UKMHomingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKMHomingComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Homing")
	bool bEnableHoming = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Homing")
	bool bAutoFindTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Homing")
	float SearchRadius = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Homing")
	float HomingAcceleration = 25000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Homing")
	float HomingStartDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Homing")
	float MaxTravelDistance = 10000.f;

	UPROPERTY(BlueprintReadOnly, Category="Homing")
	AActor* TargetActor = nullptr;

	UFUNCTION(BlueprintCallable)
	void SetTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable)
	void EnableHoming();

	UFUNCTION(BlueprintCallable)
	void DisableHoming();

protected:

	UProjectileMovementComponent* ProjectileMovement;

	FVector SpawnLocation;

	float CurrentTravelDistance = 0.f;

	FVector PreviousLocation;

	bool bHomingActivated = false;

	void FindTarget();

	void UpdateTravelDistance();

	void ActivateHoming();
};