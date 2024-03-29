#ifndef app_state_comp_h_
#define app_state_comp_h_

#include "../ecs.h"

enum AppMode
{
    CLOSEST_BATTLE,
    TV_POINT_FILL,
    PASSIVE,
    HIGHLIGHT_REP,
    INCIDENT_CAM,
    LEADER_CAM,
    EXITING_CAM,
    MODE_COUNT
};

struct ApplicationStateComponent
{
    ECS_DECLARE_TYPE;

    ApplicationStateComponent() : mode(AppMode::PASSIVE), trackPointer(1) {}

    AppMode mode;
    float trackPointer;
};
ECS_DEFINE_TYPE(ApplicationStateComponent);
typedef std::shared_ptr<ApplicationStateComponent> ApplicationStateComponentSP;

#endif