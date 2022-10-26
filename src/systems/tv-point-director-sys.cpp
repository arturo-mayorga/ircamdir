#include "tv-point-director-sys.h"
#include "../components/car-comp.h"
#include "../components/cam-ctrl-comp.h"

#include <iostream>

TvPointDirectorSystem::~TvPointDirectorSystem() {}

void TvPointDirectorSystem::configure(class ECS::World *world) {}
void TvPointDirectorSystem::unconfigure(class ECS::World *world) {}

void TvPointDirectorSystem::tick(class ECS::World *world, float deltaTime)
{
    float unmetMaxPctDelta = 0;
    int positionForMaxUnmet = -1;
    world->each<BroadcastCarSummaryComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<BroadcastCarSummaryComponentSP> cStateH)
        {
            BroadcastCarSummaryComponentSP cState = cStateH.get();
            if (cState->tvPtsPct - cState->scrTimePct > unmetMaxPctDelta)
            {
                ECS::ComponentHandle<DynamicCarStateComponentSP> dStateH = ent->get<DynamicCarStateComponentSP>();
                if (dStateH.isValid())
                {
                    DynamicCarStateComponentSP dState = dStateH.get();

                    if (!dState->isInPits && dState->deltaLapDistPct > 0)
                    {

                        unmetMaxPctDelta = cState->tvPtsPct - cState->scrTimePct;
                        positionForMaxUnmet = dState->officialPos;
                    }

                    // std::cout << cState->tvPtsPct << "  " << cState->scrTimePct << "  " << cState->tvPtsPct << "  " << unmetMaxPctPts << std::endl;
                }
            }
        });

    world->each<CameraControlComponentSP>(
        [&](ECS::Entity *ent, ECS::ComponentHandle<CameraControlComponentSP> cStateH)
        {
            CameraControlComponentSP cState = cStateH.get();
            if (positionForMaxUnmet > 0) // make sure we don't try to select the pace car
            {
                cState->tvPointsTarget = positionForMaxUnmet;
            }
        });
}