#ifndef ecs_util_h_
#define ecs_util_h_

#include "ecs.h"

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