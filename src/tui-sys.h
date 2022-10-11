#ifndef tui_sys_h_
#define tui_sys_h_

#include "ecs.h"

class TuiSystem : public ECS::EntitySystem
{
public:
    virtual ~TuiSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

private:
    void _drawScreen(class ECS::World *world);
};

#endif