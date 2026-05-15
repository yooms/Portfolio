#include "Character/Animation/MotionMatchingComponent.h"
#include "GameFramework/Character.h"
#include "Character/Animation/MotionTrajectoryComponent.h"


FPoseFeature UMotionMatchingComponent::BuildQuery()
{
    FPoseFeature Feature;

    ACharacter* Char = Cast<ACharacter>(GetOwner());
    if(!IsValid(Char))
        return Feature;

    Feature.RootPosition = Char->GetActorLocation();
    Feature.Velocity = Char->GetVelocity();

    USkeletalMeshComponent* Mesh = Char->GetMesh();
    if(!IsValid(Mesh))
        return Feature;

    Feature.BonePositions.Add(Mesh->GetSocketLocation("foot_l"));
    Feature.BonePositions.Add(Mesh->GetSocketLocation("foot_r"));

    if (UMotionTrajectoryComponent* Traj = Char->FindComponentByClass<UMotionTrajectoryComponent>())
    {
        Feature.Trajectory = Traj->GetTrajectory();
    }

    return Feature;
}

void UMotionMatchingComponent::UpdateMotionMatching(float DeltaTime)
{
    if (!Database)
        return;

    ACharacter* Char = Cast<ACharacter>(GetOwner());
    if (!Char)
        return;

    FPoseFeature Query = BuildQuery();
    FKMPoseData BestPose = Database->FindBestMatch(Query);

    if (BestPose.Animation)
    {
        UAnimInstance* Anim = Char->GetMesh()->GetAnimInstance();
        if (IsValid(Anim))
        {
            Anim->PlaySlotAnimationAsDynamicMontage(BestPose.Animation, "DefaultSlot");
        }
    }
}

