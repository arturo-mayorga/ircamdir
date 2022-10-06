#ifndef irtelemetry_sys_h_
#define irtelemetry_sys_h_

#include "ecs.h"
#include <string>
#include <memory>

enum CarType
{
    PACE,
    RACE,
    OTHER
};

struct StaticCarStateComponent
{
    ECS_DECLARE_TYPE;

    StaticCarStateComponent() : idx(-1), uid(-1), name("") {}

    int idx;
    int uid;
    std::string name;
};
ECS_DEFINE_TYPE(StaticCarStateComponent);
typedef std::shared_ptr<StaticCarStateComponent> StaticCarStateComponentSP;

struct DynamicCarStateComponent
{
    ECS_DECLARE_TYPE;

    DynamicCarStateComponent() : progress(-1), idx(-1), officialPos(0) {}

    float progress;
    int idx;
    int officialPos;
};
ECS_DEFINE_TYPE(DynamicCarStateComponent);
typedef std::shared_ptr<DynamicCarStateComponent> DynamicCarStateComponentSP;

class IrTelemetrySystem : public ECS::EntitySystem
{
public:
    virtual ~IrTelemetrySystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif