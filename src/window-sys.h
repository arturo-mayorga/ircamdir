#ifndef window_sys_h_
#define window_sys_h_

#include "ecs.h"

class WindowSystem : public ECS::EntitySystem
{
public:
    virtual ~WindowSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif