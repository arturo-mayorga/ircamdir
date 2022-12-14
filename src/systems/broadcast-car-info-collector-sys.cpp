#include "broadcast-car-info-collector-sys.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"
#include "../components/session-comp.h"

#include <iostream>
#include <iomanip>
#include <map>

BroadcastCarInfoCollectorSystem::~BroadcastCarInfoCollectorSystem()
{
}

void BroadcastCarInfoCollectorSystem::configure(class ECS::World *world)
{
}

void BroadcastCarInfoCollectorSystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

int getCurrentCameraTarget(ECS::World *world)
{
    int currentCameraTarget = -1;
    world->each<CameraActualsComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraActualsComponentSP> cStateH)
        {
            CameraActualsComponentSP cState = cStateH.get();
            currentCameraTarget = cState->currentCarIdx;
        });
    return currentCameraTarget;
}

void updateScreenAndStandingsTime(ECS::World *world, float deltaTime, int currentCameraTarget)
{
    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();

            int cIdx = cState->idx;

            ECS::ComponentHandle<BroadcastCarInfoComponentSP> bStateH = ent->get<BroadcastCarInfoComponentSP>();

            if (bStateH.isValid() && !cState->isInPits && cState->deltaLapDistPct > 0)
            {
                BroadcastCarInfoComponentSP bState = bStateH.get();

                if (currentCameraTarget == cState->idx)
                {
                    float cTime = 0;
                    bState->scrTime += deltaTime;
                }

                if (cState->officialPos > 0)
                {
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
            }
        });
}

float getTotalScreenTime(ECS::World *world)
{
    float totalScrTime = 0;
    world->each<BroadcastCarInfoComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarInfoComponentSP> cStateH)
        {
            BroadcastCarInfoComponentSP cState = cStateH.get();
            totalScrTime += cState->scrTime;
        });

    return totalScrTime;
}

void calculateTvPoints(ECS::World *world, float totalScrTime)
{
    world->each<BroadcastCarInfoComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarInfoComponentSP> cStateH)
        {
            BroadcastCarInfoComponentSP cState = cStateH.get();

            if (cState->idx == 0 || totalScrTime == 0) // the pace car doesn't get points
            {
                cState->tvPoints = 0;
            }
            else
            {
                cState->tvPoints = 1 + ((cState->top20Time + cState->top10Time + cState->top5Time + cState->top3Time + cState->leadTime) / totalScrTime);
            }
        });
}

void resetBroadcastInfo(ECS::World *world)
{
    world->each<BroadcastCarInfoComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarInfoComponentSP> bStateH)
        {
            BroadcastCarInfoComponentSP bState = bStateH.get();

            bState->scrTime = 0;
            bState->leadTime = 0;
            bState->top3Time = 0;
            bState->top5Time = 0;
            bState->top10Time = 0;
            bState->top20Time = 0;
            bState->tvPoints = 1;
        });
}

int getCurrentSessionNum(ECS::World *world)
{
    int currentSessionNum = -1;

    world->each<SessionComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<SessionComponentSP> sCompH)
        {
            SessionComponentSP sComp = sCompH.get();
            currentSessionNum = sComp->num;
        });

    return currentSessionNum;
}

void BroadcastCarInfoCollectorSystem::tick(class ECS::World *world, float deltaTime)
{
    static int lastSessionNum = -1;

    int currentSessionNum = getCurrentSessionNum(world);

    if (lastSessionNum != currentSessionNum)
    {
        resetBroadcastInfo(world);
    }

    lastSessionNum = currentSessionNum;

    updateScreenAndStandingsTime(world, deltaTime, getCurrentCameraTarget(world));
    calculateTvPoints(world, getTotalScreenTime(world));
}
