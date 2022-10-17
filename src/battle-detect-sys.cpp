#include "battle-detect-sys.h"
#include "car-comp.h"
#include "cam-ctrl-comp.h"

#include <iostream>
#include <algorithm>

BattleDetectSystem::~BattleDetectSystem()
{
}

void BattleDetectSystem::configure(class ECS::World *world)
{
}

void BattleDetectSystem::unconfigure(class ECS::World *world)
{
    world->unsubscribeAll(this);
}

#define MIN_PCT_DELTA 0.005

void BattleDetectSystem::tick(class ECS::World *world, float deltaTime)
{
    std::vector<DynamicCarStateComponentSP> states;

    world->each<DynamicCarStateComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<DynamicCarStateComponentSP> cStateH)
        {
            DynamicCarStateComponentSP cState = cStateH.get();
            // std::cout << "pit info: " << cState->isInPits << std::endl;

            if (cState->isInPits == 0 && cState->deltaLapDistPct * 100000 > MIN_PCT_DELTA)
            {
                // ignore cars if they are in the pits or moving too slow
                states.push_back(cState);
            }
        });

    // sort the states by track position
    std::sort(states.begin(), states.end(), [](DynamicCarStateComponentSP &a, DynamicCarStateComponentSP &b)
              { return a->lapDistPct > b->lapDistPct; });

    float minInterval = 2;
    int minIntervalPos = -1;

    int prevIdx = -1;
    float prevLapDistPct = -1;

    for (auto c : states)
    {
        if (prevIdx == -1)
        {
            prevIdx = c->idx;
            prevLapDistPct = c->lapDistPct;
            continue;
        }

        if (prevLapDistPct < 0 || c->lapDistPct < 0)
        {
            continue;
        }

        auto currentInterval = prevLapDistPct - c->lapDistPct;

        if (currentInterval < minInterval)
        {
            minInterval = currentInterval;
            minIntervalPos = c->officialPos;
        }

        prevIdx = c->idx;
        prevLapDistPct = c->lapDistPct;

        // std::cout << prevLapDistPct << " " << c->lapDistPct << std::endl;
    }

    world->each<CameraControlComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraControlComponentSP> cStateH)
        {
            CameraControlComponentSP cState = cStateH.get();
            if (minIntervalPos > 0) // make sure we don't try to select the pace car
            {
                cState->targetCarPosRequested = minIntervalPos;
            }
        });

    // std::cout << "interval: " << minInterval << " at: " << prevIdx << std::endl;

    // std::cout << std::endl
    //           << std::endl;
}
