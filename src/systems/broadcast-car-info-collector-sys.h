#ifndef broadcast_car_info_collector_system_h_
#define broadcast_car_info_collector_system_h_

#include "../ecs.h"

class BroadcastCarInfoCollectorSystem : public ECS::EntitySystem
{
public:
    virtual ~BroadcastCarInfoCollectorSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif