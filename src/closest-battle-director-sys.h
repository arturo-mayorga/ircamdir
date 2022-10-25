#ifndef closest_battle_director_sys_h_
#define closest_battle_director_sys_h_

#include "ecs.h"

class ClosestBattleDirectorSystem : public ECS::EntitySystem
{
public:
    virtual ~ClosestBattleDirectorSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif