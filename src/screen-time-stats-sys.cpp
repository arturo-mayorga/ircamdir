#include "screen-time-stats-sys.h"
#include "car-comp.h"
#include "cam-ctrl-comp.h"

#include <iostream>
#include <iomanip>
#include <map>

ScreenTimeStatsSys::~ScreenTimeStatsSys()
{
}

void ScreenTimeStatsSys::configure(class ECS::World *world)
{
}

void ScreenTimeStatsSys::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

void ScreenTimeStatsSys::tick(class ECS::World *world, float deltaTime)
{
    // figure out who has the current camera
    int currentCameraTarget = -1;
    world->each<CameraControlComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraControlComponentSP> cStateH)
        {
            CameraControlComponentSP cState = cStateH.get();
            currentCameraTarget = cState->targetCarPosActual;
        });

    // update screen and standings time
    int currentCarIndex = -1;
    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();

            int cIdx = cState->idx;

            ECS::ComponentHandle<BroadcastCarInfoComponentSP> bStateH = ent->get<BroadcastCarInfoComponentSP>();

            if (bStateH.isValid())
            {
                BroadcastCarInfoComponentSP bState = bStateH.get();

                if (currentCameraTarget == cState->officialPos)
                {
                    float cTime = 0;
                    bState->scrTime += deltaTime;
                }

                if (cState->officialPos <= 1)
                {
                    bState->leadTime += deltaTime;
                }
                if (cState->officialPos <= 3)
                {
                    bState->top3Time += deltaTime;
                }
                if (cState->officialPos <= 5)
                {
                    bState->top5Time += deltaTime;
                }
                if (cState->officialPos <= 10)
                {
                    bState->top10Time += deltaTime;
                }
                if (cState->officialPos <= 20)
                {
                    bState->top20Time += deltaTime;
                }
            }
        });

    // figure out the total screen time
    float totalScrTime = 0;
    world->each<BroadcastCarInfoComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarInfoComponentSP> cStateH)
        {
            BroadcastCarInfoComponentSP cState = cStateH.get();
            totalScrTime += cState->scrTime;
        });

    // calculate the tv points
    world->each<BroadcastCarInfoComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarInfoComponentSP> cStateH)
        {
            BroadcastCarInfoComponentSP cState = cStateH.get();
            cState->tvPoints = 1 + ((cState->top20Time + cState->top10Time + cState->top5Time + cState->top3Time + cState->leadTime) / totalScrTime);

            if (cState->idx == 0) // the pace car doesn't get points
            {
                cState->tvPoints = 0;
            }
        });
}
