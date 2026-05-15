// PoseDatabase.cpp

#include "Character/Animation/PoseDatabase.h"

static float ComputeCost(const FPoseFeature& A, const FPoseFeature& B)
{
    float Cost = 0.f;

    Cost += FVector::DistSquared(A.RootPosition, B.RootPosition);
    Cost += FVector::DistSquared(A.Velocity, B.Velocity);

    int32 BoneCount = FMath::Min(A.BonePositions.Num(), B.BonePositions.Num());

    for (int32 i = 0; i < BoneCount; i++)
    {
        Cost += FVector::DistSquared(A.BonePositions[i], B.BonePositions[i]);
    }

    int32 TrajCount = FMath::Min(A.Trajectory.Num(), B.Trajectory.Num());

    for (int32 i = 0; i < TrajCount; i++)
    {
        Cost += FVector::DistSquared(A.Trajectory[i].Position, B.Trajectory[i].Position);
    }

    return Cost;
}

FKMPoseData UPoseDatabase::FindBestMatch(const FPoseFeature& Query)
{
    float BestCost = FLT_MAX;
    FKMPoseData BestPose;

    for (const FKMPoseData& Pose : Poses)
    {
        float Cost = ComputeCost(Query, Pose.Feature);

        if (Cost < BestCost)
        {
            BestCost = Cost;
            BestPose = Pose;
        }
    }

    return BestPose;
}
