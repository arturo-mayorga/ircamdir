#ifndef tv_point_selector_h_
#define tv_point_selector_h_

#include "ecs.h"

class TvPointSelectorSystem : public ECS::EntitySystem
{
public:
    virtual ~TvPointSelectorSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif