#ifndef irtelemetry_sys_h_
#define irtelemetry_sys_h_

#include "../ecs.h"
#include "../components/cam-ctrl-comp.h"

class IrTelemetrySystem : public ECS::EntitySystem,
                          public ECS::EventSubscriber<OnCameraChangeRequest>,
                          public ECS::EventSubscriber<OnFrameNumChangeRequest>
{
public:
    virtual ~IrTelemetrySystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;

    virtual void receive(ECS::World *world, const OnCameraChangeRequest &event) override;

    virtual void receive(ECS::World *world, const OnFrameNumChangeRequest &event) override;
};

#endif