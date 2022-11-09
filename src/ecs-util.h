#ifndef ecs_util_h_
#define ecs_util_h_

#include "ecs.h"

#define ECS_MINIMAL_SYSTEM(systemName)                                        \
    class systemName : public ECS::EntitySystem                               \
    {                                                                         \
    public:                                                                   \
        virtual ~##systemName();                                              \
                                                                              \
        virtual void configure(class ECS::World *world) override;             \
                                                                              \
        virtual void unconfigure(class ECS::World *world) override;           \
                                                                              \
        virtual void tick(class ECS::World *world, float deltaTime) override; \
    };

// To create a stub implementation of a minimal system
// you can use the following code snippet
// ---------------------------------------------------
// ##systemName::~##systemName() {}
// void ##systemName::configure(class ECS::World *world) {}
// void ##systemName::unconfigure(class ECS::World *world) {}
// void ##systemName::tick(class ECS::World *world, float deltaTime) {}

namespace ECSUtil
{
    template <typename CompType>
    CompType getFirstCmp(ECS::World *w)
    {
        CompType r;
        int first = 1;

        w->each<CompType>(
            [&](ECS::Entity *ent, ECS::ComponentHandle<CompType> cStateH)
            {
                if (first)
                {
                    first = 0;
                    r = cStateH.get();
                }
            });

        return r;
    }

}

#endif