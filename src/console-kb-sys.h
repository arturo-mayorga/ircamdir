#ifndef console_kb_sys_h_
#define console_kb_sys_h_

#include "ecs.h"

class ConsoleKbSystem : public ECS::EntitySystem
{
public:
    virtual ~ConsoleKbSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif