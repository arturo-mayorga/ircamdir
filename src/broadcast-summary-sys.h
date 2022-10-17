#ifndef broadcast_summary_sys_h_
#define broadcast_summary_sys_h_

#include "ecs.h"

class BroadcastSummarySystem : public ECS::EntitySystem
{
public:
    virtual ~BroadcastSummarySystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif