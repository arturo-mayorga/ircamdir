#include "broadcast-summary-sys.h"
#include "car-comp.h"
#include "cam-ctrl-comp.h"

BroadcastSummarySystem::~BroadcastSummarySystem()
{
}

void BroadcastSummarySystem::configure(class ECS::World *world)
{
}

void BroadcastSummarySystem::unconfigure(class ECS::World *world)
{
}

void BroadcastSummarySystem::tick(class ECS::World *world, float deltaTime)
{
    float totalTime = 0;
    float maxScrTime = 0;
    float totalTvPts = 0;
    float maxTvPts = 0;

    world->each<BroadcastCarInfoComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarInfoComponentSP> bStateH)
        {
            BroadcastCarInfoComponentSP bState = bStateH.get();
            totalTime += bState->scrTime;
            totalTvPts += bState->tvPoints;

            if (maxScrTime < bState->scrTime)
            {
                maxScrTime = bState->scrTime;
            }

            if (maxTvPts < bState->tvPoints)
            {
                maxTvPts = bState->tvPoints;
            }
        });

    world->each<BroadcastCarInfoComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarInfoComponentSP> bStateH)
        {
            BroadcastCarInfoComponentSP bState = bStateH.get();

            ECS::ComponentHandle<BroadcastCarSummaryComponentSP> bSummH = ent->get<BroadcastCarSummaryComponentSP>();
            if (bSummH.isValid())
            {
                BroadcastCarSummaryComponentSP bSumm = bSummH.get();

                bSumm->tvPtsPct = bState->tvPoints / totalTvPts;
                bSumm->scrTimePct = bState->scrTime / totalTime;
            }
        });
}
