#ifndef battle_detect_sys_h_
#define battle_detect_sys_h_

#include "ecs.h"

class BattleDetectSystem : public ECS::EntitySystem
{
public:
    virtual ~BattleDetectSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif