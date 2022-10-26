#ifndef tv_point_director_sys_h_
#define tv_point_director_sys_h_

#include "../ecs.h"

class TvPointDirectorSystem : public ECS::EntitySystem
{
public:
    virtual ~TvPointDirectorSystem();

    virtual void configure(class ECS::World *world) override;

    virtual void unconfigure(class ECS::World *world) override;

    virtual void tick(class ECS::World *world, float deltaTime) override;
};

#endif