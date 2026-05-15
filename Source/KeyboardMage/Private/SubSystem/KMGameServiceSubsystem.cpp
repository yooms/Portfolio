#include "SubSystem/KMGameServiceSubsystem.h"

void UKMGameServiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    OnInit();
}

void UKMGameServiceSubsystem::Deinitialize()
{
    OnShutdown();

    Super::Deinitialize();
}