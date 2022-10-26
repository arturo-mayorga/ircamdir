#ifndef screen_time_stats_sys_h_
#define screen_time_stats_sys_h_

#include "../ecs.h"

class ScreenTimeStatsSys : public ECS::EntitySystem
{
public:
    virtual ~ScreenTimeStatsSys();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif