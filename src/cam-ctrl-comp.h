#ifndef cam_ctrl_comp_h_
#define cam_ctrl_comp_h_

#include "ecs.h"
#include <memory>

struct CameraControlComponent
{
    ECS_DECLARE_TYPE;

    CameraControlComponent() : targetCarPosActual(-1), targetCarPosRequested(-1), closestBattleTarget(-1), tvPointsTarget(-1) {}

    int targetCarPosActual;
    int targetCarPosRequested;
    int closestBattleTarget;
    int tvPointsTarget;
};
ECS_DEFINE_TYPE(CameraControlComponent);
typedef std::shared_ptr<CameraControlComponent> CameraControlComponentSP;

#endif