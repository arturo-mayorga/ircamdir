#ifndef head_of_direction_sys_h_
#define head_of_direction_sys_h_

#include "ecs.h"

class HeadOfDirectionSystem : public ECS::EntitySystem
{
public:
    virtual ~HeadOfDirectionSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif