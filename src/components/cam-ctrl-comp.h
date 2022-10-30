#ifndef cam_ctrl_comp_h_
#define cam_ctrl_comp_h_

#include "../ecs.h"
#include <memory>

enum SpecialCarNum
{
    // note that this doesn't match irsdk_csMode to ensure avoidint
    // cross dependencies
    EXITING = -3,
    INCIDENT = -2,
    LEADER = -1,
    // DRIVER + car number...
    DRIVER = 0
};

struct CameraRequestComponent
{
    ECS_DECLARE_TYPE;

    CameraRequestComponent() : changeThisFrame(0), targetCarIdx(-1) {}

    int changeThisFrame;
    int targetCarIdx;
};
ECS_DEFINE_TYPE(CameraRequestComponent);
typedef std::shared_ptr<CameraRequestComponent> CameraRequestComponentSP;

struct CameraActualsComponent
{
    ECS_DECLARE_TYPE;

    CameraActualsComponent() : currentCarIdx(-1), timeSinceLastChange(0) {}

    int currentCarIdx;
    float timeSinceLastChange;
};
ECS_DEFINE_TYPE(CameraActualsComponent);
typedef std::shared_ptr<CameraActualsComponent> CameraActualsComponentSP;

struct CameraDirectionSubTargetsComponent
{
    ECS_DECLARE_TYPE;

    CameraDirectionSubTargetsComponent() : closestBattleCarIdx(-1), tvPointsCarIdx(-1) {}

    int closestBattleCarIdx;
    int tvPointsCarIdx;
};
ECS_DEFINE_TYPE(CameraDirectionSubTargetsComponent);
typedef std::shared_ptr<CameraDirectionSubTargetsComponent> CameraDirectionSubTargetsComponentSP;

#endif