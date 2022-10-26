#ifndef app_state_comp_h_
#define app_state_comp_h_

#include "../ecs.h"

enum AppMode
{
    CLOSEST_BATTLE,
    TV_POINT_FILL,
    HOLD_TRACK_POS,
    PASSIVE,
    MODE_COUNT
};

struct ApplicationStateComponent
{
    ECS_DECLARE_TYPE;

    ApplicationStateComponent() : mode(AppMode::CLOSEST_BATTLE), trackPointer(1) {}

    AppMode mode;
    float trackPointer;
};
ECS_DEFINE_TYPE(ApplicationStateComponent);
typedef std::shared_ptr<ApplicationStateComponent> ApplicationStateComponentSP;

#endif