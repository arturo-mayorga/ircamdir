#include "broadcast-summary-sys.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"

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

                bSumm->tvPtsPct = (totalTvPts != 0) ? bState->tvPoints / totalTvPts : 0;
                bSumm->scrTimePct = (totalTime != 0) ? bState->scrTime / totalTime : 0;
            }
        });
}
