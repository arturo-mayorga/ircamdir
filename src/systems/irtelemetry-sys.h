#ifndef irtelemetry_sys_h_
#define irtelemetry_sys_h_

#include "../ecs.h"

class IrTelemetrySystem : public ECS::EntitySystem
{
public:
    virtual ~IrTelemetrySystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif