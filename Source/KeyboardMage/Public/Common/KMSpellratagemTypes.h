#pragma once
#include "CoreMinimal.h"
#include "KMSpellratagemTypes.generated.h"

UENUM(BlueprintType)
enum class EKMCommandDirection : uint8
{
    Up,
    Down,
    Left,
    Right
};

USTRUCT(BlueprintType)
struct FStratagemCommand
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    TArray<EKMCommandDirection> Inputs;

    bool IsEmpty() const { return Inputs.Num() == 0; }

    bool Matches(const TArray<EKMCommandDirection>& Other) const
    {
        if (Inputs.Num() != Other.Num()) return false;
        for (int32 i = 0; i < Inputs.Num(); i++)
            if (Inputs[i] != Other[i]) return false;
        return true;
    }
};
